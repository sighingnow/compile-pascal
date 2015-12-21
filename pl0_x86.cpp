#include "pl0_x86.h"

static IOOut out;

// static int p = 0;
static int dist = 0;
static std::vector<int> old;
static pl0_env<LOC> runtime;
static SimpleAllocator manager(runtime, out, dist);
static vector<pair<string, string>> asciis;

static size_t pl0_x86_gen_param(std::vector<TAC> & code, size_t p = 0) {
    int size = 8; // the first argument: ebp+8
    while (code[p].op == "param" || code[p].op == "paramref") {
        runtime.push(LOC(code[p].rd->sv, size, code[p].op == "paramref"));
        p = p + 1; size = size + 4;
    }
    return p;
}

static std::string x86_gen_def(TAC & code) {
    int size = 0;
    if (code.rs->sv == "integer") {
        size = code.rt->iv == -1 ? 4 : (code.rt->iv * 4);
    }
    else {
        size = code.rt->iv == -1 ? 4 : (code.rt->iv * 4); // TODO
    }
    dist -= size;
    runtime.push(LOC(code.rd->sv, dist));
    return string("    sub esp, ") + to_string(size) + "\t\t;; " + code.str();
}

static void pl0_x86_gen_header(BasicBlock & bb, std::vector<std::string> & buffer) {
    runtime.tag(); old.emplace_back(dist); dist = 0;
    size_t p = pl0_x86_gen_param(bb.code, 0);
    buffer.emplace_back(string(""));
    buffer.emplace_back(string("    global ") + bb.code[p].rd->sv);
    buffer.emplace_back(string(bb.code[p].rd->sv) + ":");
    buffer.emplace_back(string("    push ebp"));
    buffer.emplace_back(string("    mov ebp, esp"));
    // prepare displays.
    int d = runtime.depth();
    runtime.push(LOC(string("$fn") + bb.code[p].rd->sv, d));
    buffer.emplace_back("    mov eax, dword [ebp]");
    for (int i = 1; i < d; ++i) {
        buffer.emplace_back("    push dword [eax-" + to_string(i*4) + "]");
    }
    dist = dist - 4 * d;
    buffer.emplace_back("    push ebp");
    if (++p < bb.size() && bb.code[p].op == "allocret") {
        dist = dist - 4;
        runtime.push(LOC(bb.code[p].rd->sv, dist));
        buffer.emplace_back(string("    sub esp, ") + to_string(4) + "\t\t;; " + bb.code[p].str());
        p = p + 1;
    }
    while (p < bb.size() && bb.code[p].op == "def") {
        buffer.emplace_back(x86_gen_def(bb.code[p++]));
    }
}

static void pl0_x86_gen_common(TAC & c) {
    if (c.op == "endproc" || c.op == "endfunc") {
        manager.release("eax", true);
        manager.spillAll();
        out.emit(string("    leave"));
        out.emit(string("    ret"), c);
        runtime.detag();
        old.pop_back(); dist = old.back(); old.pop_back();
    }
    else if (c.op == "=") {
        std::string rs, rd = manager.alloc(c.rd->sv);
        if (c.rs->t == Value::TYPE::IMM) {
            rs = c.rs->value();
        }
        else {
            rs = manager.locate(c.rs->sv);
        }
        out.emit(string("    mov ") + rd + ", " + rs, c);
        if (c.rd->dt == "char") {
            out.emit(string("    and ") + rd + ", 0x000000ff");
        }
        manager.spill(rd);
    }
    else if (c.op == "[]=") {
        std::string rt, rs, rd;
        if (c.rs->t == Value::TYPE::IMM) {
            rs = c.rs->value();
        }
        else {
            rs = manager.load(c.rs->sv, "edi");
        }
        if (c.rt->t == Value::TYPE::IMM) {
            rt = c.rt->value();
        }
        else {
            rt = manager.load(c.rt->sv, "esi");
        }
        rd = manager.locate(c.rd->sv);
        if (rd.length() >= 3 && rd.substr(0, 3) == "dwo") {
            rd = rd.substr(0, rd.length()-1) + "+4*" + rs + "]";
        }
        else {
            rd = string("dword [") + rd + "+4*" + rs + "]"; 
        }
        out.emit(string("    mov ") + rd + ", " + rt, c);
        manager.spill(rd);
    }
    else if (c.op == "=[]") {
        std::string rt, rs, rd = manager.load(c.rd->sv, "esi");
        if (c.rt->t == Value::TYPE::IMM) {
            rt = c.rt->value();
        }
        else {
            rt = manager.load(c.rt->sv, "edi");
        }
        rs = manager.locate(c.rs->sv);
        if (rs.length() >= 3 && rs.substr(0, 3) == "dwo") {
            rs = rs.substr(0, rs.length()-1) + "+4*" + rt + "]";
        }
        else {
            rs = string("dword [") + rs + "+4*" + rt + "]"; 
        }
        out.emit(string("    mov ") + rd + ", " + rs, c);
        manager.spill(rd);
    }
    else if (c.op == "loadret") {
        manager.spillAll();
        out.emit("    mov eax, dword [ebp-" + to_string(runtime.depth()*4+4) + "]", c);
    }
    else if (c.op == "exit") {
        out.emit(string("    mov eax, ") + c.rd->value(), c);
    }
    else if (c.op == "call") {
        manager.spillAll();
        for (auto && a: c.args) { // push
            if (a.second) { // call by reference
                if (a.first->sv.back() == '#') {
                    // array element.
                    std::string t = a.first->sv, array, idx;
                    t = t.substr(0, t.length()-1);
                    int p = t.rfind('#');
                    idx = t.substr(p+1);
                    t = t.substr(0, p);
                    p = t.rfind('#');
                    if (idx.front() >= '0' && idx.front() <= '9') {
                        out.emit(string("    mov edi, ") + idx);
                    }
                    else {
                        manager.load(idx, "edi");
                        manager.release("edi", true);
                    }
                    array = manager.locate(t.substr(p+1));
                    if (array.length() >= 3 && array.substr(0, 3) == "dwo") {
                        array = array.substr(6, array.length()-7) + "+4*" + idx + "]";
                    }
                    else {
                        array = string("[") + array + "+4*" + idx + "]"; 
                    }
                    out.emit(string("    lea eax, ") + array);
                    out.emit(string("    push eax"), c);
                }
                else {
                    manager.store(a.first->sv);
                    out.emit(string("    lea eax, ") + manager.addr(a.first->sv));
                    out.emit(string("    push eax"));
                }
            }
            else { // call by value.
                if (a.first->t == Value::TYPE::IMM) {
                    out.emit(string("    push ") + a.first->value());
                }
                else if (manager.exist(a.first->sv).length() == 0) {
                    out.emit(string("    push ") + manager.locate(a.first->sv));
                }
            }
        }
        out.emit("    call " + c.rd->sv, c);
        if (c.rt) { // for function call, load return value.
            manager.remap("eax", c.rt->sv);
        }
        if (c.args.size() > 0) { // pop
            out.emit("    add esp, " + to_string(c.args.size() * 4));
        }
        // at the end of function call, merge two frame, so, don't restore $esp value.
    }
    else if (c.op == "read") {
        manager.spill("eax");
        manager.spill("ecx");
        manager.spill("edx");
        manager.store(c.rd->sv);
        out.emit(string("    lea eax, ") + manager.addr(c.rd->sv));
        out.emit(string("    push dword eax"));
        if (c.rd->dt == "integer") {
            out.emit(string("    push dword __fin_int"));
        }
        else {
            out.emit(string("    push dword __fin_char"));
        }
        out.emit(string("    call _scanf"), c);
        out.emit(string("    add esp, 8\t\t;; pop stack at once."));
    }
    else if (c.op == "write_e") {
        manager.spill("eax");
        manager.spill("ecx");
        manager.spill("edx");
        if (c.rd->t == Value::TYPE::IMM) {
            out.emit(string("    push ") + c.rd->value());
        }
        else {
            out.emit(string("    push ") + manager.locate(c.rd->sv));
        }
        if (c.rd->dt == "integer") {
            out.emit(string("    push dword __fout_int"));
        }
        else {
            out.emit(string("    push dword __fout_char"));
        }
        out.emit(string("    call    _printf"), c);
        out.emit(string("    add esp, 8\t\t;; pop stack at once."));
    }
    else if (c.op == "write_s") {
        manager.spill("eax");
        manager.spill("ecx");
        manager.spill("edx");
        asciis.emplace_back(make_pair(c.rd->sv, c.rs->value()));
        out.emit(string("    push dword __L") + c.rs->value());
        out.emit(string("    push dword __fout_string"));
        out.emit(string("    call    _printf"), c);
        out.emit(string("    add esp, 8\t\t;; pop stack at once."));
    }
    else if (c.op == "+") {
        std::string rd = c.rd->sv, rs = c.rs->value(), rt = c.rt->value();
        std::string dest = manager.load(rd);
        if (c.rs->t == Value::TYPE::IMM && c.rt->t == Value::TYPE::IMM) {
            out.emit(string("    mov ") + dest + ", " + rs);
            out.emit(string("    add ") + dest + ", " + rt, c);
        }
        else if (c.rs->t == Value::TYPE::IMM) {
            if (rd != rt) {
                out.emit(string("    mov ") + dest + ", " + manager.locate(rt));
            }
            out.emit(string("    add ") + dest + ", " + rs, c);
        }
        else if (c.rt->t == Value::TYPE::IMM) {
            if (rd != rs) {
                out.emit(string("    mov ") + dest + ", " + manager.locate(rs));
            }
            out.emit(string("    add ") + dest + ", " + rt, c);
        }
        else {
            if (rd != rs && rd != rt) {
                out.emit(string("    mov ") + dest + ", " + manager.locate(rs));
            }
            if (rd == rt) {
                out.emit(string("    add ") + dest + ", " + manager.locate(rs), c);
            }
            else {
                out.emit(string("    add ") + dest + ", " + manager.locate(rt), c);
            }                
        }
    }
    else if (c.op == "-") {
        std::string rd = c.rd->sv, rs = c.rs->value(), rt = c.rt->value();
        std::string dest = manager.load(rd);
        if (rs == rt) {
            out.emit(string("    mov ") + dest + ", 0", c);
        }
        else if (c.rs->t == Value::TYPE::IMM && c.rt->t == Value::TYPE::IMM) {
            out.emit(string("    mov ") + dest + ", " + rs);
            out.emit(string("    sub ") + dest + ", " + rt, c);
        }
        else if (c.rs->t == Value::TYPE::IMM) {
            if (rd == rt) {
                out.emit(string("    neg ") + dest);
                out.emit(string("    add ") + dest + ", " + rs, c);
            }
            else {
                out.emit(string("    mov ") + dest + ", " + rs);
                out.emit(string("    sub ") + dest + ", " + manager.locate(rt), c);
            }
        }
        else if (c.rt->t == Value::TYPE::IMM) {
            if (rd != rs) {
                out.emit(string("    mov ") + dest + ", " + manager.locate(rs));
            }
            out.emit(string("    sub ") + dest + ", " + rt, c);
        }
        else {
            if (rd == rt) {
                out.emit(string("    neg ") + dest);
                out.emit(string("    add ") + dest + ", " + manager.locate(rs), c);
            }
            else {
                if (rd != rs) {
                    out.emit(string("    mov ") + dest + ", " + manager.locate(rs));
                }
                out.emit(string("    sub ") + dest + ", " + manager.locate(rt), c);
            }
        }
    }
    else if (c.op == "*") {
        manager.spill("eax");
        manager.spill("edx");
        if (c.rt->t == Value::TYPE::IMM) {
            out.emit(string("    mov edx, ") + c.rt->value());
        }
        else {
            out.emit(string("    mov edx, ") + manager.locate(c.rt->sv));
        }
        if (c.rs->t == Value::TYPE::IMM) {
            out.emit(string("    mov eax, ") + c.rs->value());
        }
        else {
            out.emit(string("    mov eax, ") + manager.locate(c.rs->sv));
        }
        out.emit(string("    imul edx"), c);
        manager.remap("eax", c.rd->sv);
    }
    else if (c.op == "/") {
        manager.spill("eax");
        manager.spill("edx");
        out.emit(string("    mov edx, 0"));
        std::string rt = c.rt->t == Value::TYPE::IMM ? c.rt->value() : manager.locate(c.rt->sv);
        if (c.rs->t == Value::TYPE::IMM) {
            out.emit(string("    mov eax, ") + c.rs->value());
        }
        else {
            out.emit(string("    mov eax, ") + manager.locate(c.rs->sv));
        }
        out.emit("    cdq");
        out.emit("    idiv " + rt, c);
        manager.remap("eax", c.rd->sv);
    }
    else if (c.op == "%") {
        manager.spill("eax");
        manager.spill("edx");
        out.emit(string("    mov edx, 0"));
        std::string rt = c.rt->t == Value::TYPE::IMM ? c.rt->value() : manager.locate(c.rt->sv);
        if (c.rs->t == Value::TYPE::IMM) {
            out.emit(string("    mov eax, ") + c.rs->value());
        }
        else {
            out.emit(string("    mov eax, ") + manager.locate(c.rs->sv));
        }
        out.emit(string("    cdq")); // Convert double-word to quad-word
        out.emit(string("    idiv ") + rt, c);
        manager.remap("edx", c.rd->sv);
    }
    else if (c.op == "cmp") {
        manager.spillAll();
        std::string comp, rs = "esi", rt = "edi";
        if (c.rs->t == Value::TYPE::IMM) {
            out.emit("    mov esi, " + c.rs->value());
        }
        else {
            manager.load(c.rs->sv, "esi");
            manager.release("esi", true);
        }
        if (c.rt->t == Value::TYPE::IMM) {
            out.emit("    mov edi, " + c.rt->value());
        }
        else {
            manager.load(c.rt->sv, "edi");
            manager.release("edi", true);
        }
        comp = string("    cmp esi, edi");
        if (old.back() - dist > 0) {
            out.emit(string("    add esp, ") + to_string(old.back() - dist));
            dist = old.back();
        }
        out.emit(comp, c);
    }
    else if (c.op == "label") {
        out.emit(string("__L") + c.rd->value() + ":");
        if (c.rs == nullptr) {
            old.emplace_back(dist);
        }
        else {
            // cout << ";; merge scope." << endl;
        }
    }
    else if (c.op == "goto") {
        manager.spillAll();
        if (old.back() - dist > 0) {
            out.emit(string("    add esp, ") + to_string(old.back() - dist));
        }
        dist = old.back(); old.pop_back();
        out.emit(string("    ") + c.rd->sv + " __L" + c.rs->value(), c);
    }
    else {
        out.emit("UNIMPLEMENT", c);
    }
}

static void pl0_x86_gen_body(BasicBlock & bb) {
    for (auto && c: bb.code) {
        pl0_x86_gen_common(c);
    }
}

static size_t pl0_x86_gen_blocks(std::vector<BasicBlock> & bbs, size_t bp = 0) {
    std::vector<std::string> buffer;
    pl0_x86_gen_header(bbs[bp++], buffer);
    runtime.dump();
    while (bp < bbs.size() && bbs[bp].no == 0) {
        bp = pl0_x86_gen_blocks(bbs, bp);
    }
    for (auto && s: buffer) {
        out.emit(s);
    }
    while (bp < bbs.size() && bbs[bp].no != 0) {
        pl0_x86_gen_body(bbs[bp]);
        if (bbs[bp++].is_end) {
            break; // the end block of a procedure or a function.
        }
    }
    return bp;
}

void pl0_x86_gen(std::string file, std::vector<BasicBlock> & bbs) {
    out.emit(string(";; file: ") + file);
    out.emit(string(""));
    out.emit(string("    bits 32\t\t;; 32bit machine"));
    out.emit(string("    cpu 686\t\t;; i686 CPU"));
    out.emit(string(""));
    out.emit(string(";; external functions (from standard C library)"));
    out.emit(string("    extern _scanf"));
    out.emit(string("    extern _printf"));
    out.emit(string(""));
    out.emit(string("section .text"));
    pl0_x86_gen_blocks(bbs);
    // dump all constant ascii string.
    out.emit(string(""));
    out.emit(string("section .data"));
    out.emit(string("    __fin_int:        db      \"%d\", 0x0"));
    out.emit(string("    __fin_char:       db      \"%c\", 0x0"));
    out.emit(string("    __fin_string:     db      \"%s\", 0x0"));
    out.emit(string("    __fout_int:       db      \"%d\", 0xA, 0x0"));
    out.emit(string("    __fout_char:      db      \"%c\", 0x0"));
    out.emit(string("    __fout_string:    db      \"%s\", 0xA, 0x0"));
    out.emit(string(""));
    out.emit(string("section .data"));
    for (auto && a: asciis) {
        out.emit(string("    __L") + a.second + ":\t\tdb\t\t\"" + a.first + "\", 0x0");
    }
}



