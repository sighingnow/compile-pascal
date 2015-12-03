#include <vector>
#include <iostream>
#include "pl0_ast.hpp"
#include "pl0_regalloc.h"
using namespace std;

static IOOut out;

static int p = 0;
static int dist = 0, old = 0;
static vector<TAC> code;
static pl0_env<LOC> runtime;
static SimpleAllocator manager(runtime, out, dist);
static vector<pair<string, string>> asciis;

std::string x86_gen_def() {
    int size = 0;
    if (code[p].rs->sv == "integer") {
        size = code[p].rt->iv == -1 ? 4 : (code[p].rt->iv * 4);
    }
    else {
        size = code[p].rt->iv == -1 ? 4 : (code[p].rt->iv * 4); // TODO
    }
    dist -= size;
    runtime.push(LOC(code[p].rd->sv, dist));
    return string("    sub esp, ") + std::to_string(size) + "\t\t;; " + code[p].str();
}

void x86_gen_param() {
    int size = 8; // the first argument: ebp+8
    while (code[p].op == "param" || code[p].op == "paramref") {
        runtime.push(LOC(code[p].rd->sv, size, code[p].op == "paramref"));
        p = p + 1; size = size + 4;
    }
}

void x86_gen_callable() {
    vector<string> buffer;
    runtime.tag();
    buffer.emplace_back("");
    old = dist; dist = 0;
    x86_gen_param();
    buffer.emplace_back(string("    global ") + code[p].rd->sv);
    buffer.emplace_back(code[p].rd->sv + ":");
    buffer.emplace_back("    push ebp");
    buffer.emplace_back("    mov ebp, esp");
    while (code[++p].op == "def") {
        buffer.emplace_back(x86_gen_def());
    }
    while (code[p].op == "param" || code[p].op == "paramref"
            || code[p].op == "function" || code[p].op == "procedure") {
        x86_gen_callable();
    }
    for (auto && cs: buffer) {
        out.emit(cs);
    }
    runtime.dump();
    while (code[p].op != "endproc" && code[p].op != "endfunc") {
        if (code[p].op == "=") {
            std::string rs, rd = manager.load(code[p].rd->sv);
            if (code[p].rs->t == Value::TYPE::INT) {
                rs = code[p].rs->value();
            }
            else {
                rs = manager.locate(code[p].rs->sv);
            }
            out.emit(string("    mov ") + rd + ", " + rs, code[p]);
        }
        else if (code[p].op == "[]=") {
            std::string rt, rs, rd;
            if (code[p].rs->t == Value::TYPE::INT) {
                rs = code[p].rs->value();
            }
            else {
                rs = manager.load(code[p].rs->sv);
            }
            if (code[p].rt->t == Value::TYPE::INT) {
                rt = code[p].rt->value();
            }
            else {
                rt = manager.load(code[p].rt->sv);
            }
            rd = manager.locate(code[p].rd->sv);
            if (rd.substr(0, 3) == "dwo") {
                rd = rd.substr(0, rd.length()-1) + "+4*" + rs + "]";
            }
            else {
                rd = string("dword [") + rd + "+4*" + rs + "]"; 
            }
            out.emit(string("    mov ") + rd + ", " + rt, code[p]);
        }
        else if (code[p].op == "=[]") {
            std::string rt, rs, rd = manager.load(code[p].rd->sv);
            if (code[p].rt->t == Value::TYPE::INT) {
                rt = code[p].rt->value();
            }
            else {
                rt = manager.load(code[p].rt->sv);
            }
            rs = manager.locate(code[p].rs->sv);
            if (rs.substr(0, 3) == "dwo") {
                rs = rs.substr(0, rs.length()-1) + "+4*" + rt + "]";
            }
            else {
                rs = string("dword [") + rs + "+4*" + rt + "]"; 
            }
            out.emit(string("    mov ") + rd + ", " + rs, code[p]);
        }
        else if (code[p].op == "allocret") {
            dist = dist - 4;
            runtime.push(LOC(code[p].rd->sv, dist));
            out.emit(string("    sub esp, ") + std::to_string(4), code[p]);
        }
        else if (code[p].op == "setret") {
            std::string ret;
            if (code[p].rd->t == Value::TYPE::INT) {
                ret = code[p].rd->value();
            }
            else {
                if (manager.exist(code[p].rd->sv).length() == 0) {
                    manager.load(code[p].rd->sv);
                }
                ret = manager.locate(code[p].rd->sv);
            }
            out.emit(string("    mov dword [ebp-4], ") + ret, code[p]);
        }
        else if (code[p].op == "exit") {
            out.emit(string("    mov eax, ") + code[p].rd->value(), code[p]);
        }
        else if (code[p].op == "loadret") {
            manager.spillAll();
            out.emit("    mov eax, dword [ebp-4]", code[p]);
        }
        else if (code[p].op == "def") {
            out.emit(x86_gen_def());
        }
        else if (code[p].op == "push") {
            manager.spillAll();
            if (code[p].rd->t == Value::TYPE::INT) {
                out.emit(string("    push ") + code[p].rd->value(), code[p]);
            }
            else if (manager.exist(code[p].rd->sv).length() == 0) {
                manager.load(code[p].rd->sv, "eax");
                out.emit(string("    push eax"), code[p]);
            }
            else {
                out.emit(string("    push ") + manager.locate(code[p].rd->sv), code[p]);
            }
        }
        else if (code[p].op == "pushref") {
            manager.spillAll();
            manager.store(code[p].rd->sv);
            out.emit(string("    lea eax, ") + manager.addr(code[p].rd->sv));
            out.emit(string("    push dword eax"), code[p]);
        }
        else if (code[p].op == "pop") {
            out.emit(string("    add esp, 4"), code[p]);
        }
        else if (code[p].op == "callfunc") {
            out.emit("    call " + code[p].rd->sv, code[p]);
            manager.remap("eax", code[p].rs->sv);
        }
        else if (code[p].op == "callproc") {
            out.emit("    call " + code[p].rd->sv, code[p]);
        }
        else if (code[p].op == "read") {
            manager.spill("eax");
            manager.spill("ecx");
            manager.spill("edx");
            manager.store(code[p].rd->sv);
            out.emit(string("    lea eax, ") + manager.addr(code[p].rd->sv));
            out.emit(string("    push dword eax"));
            out.emit(string("    push dword __fin_int"));
            out.emit(string("    call _scanf"), code[p]);
            out.emit(string("    add esp, 8\t\t;; pop stack at once."));
        }
        else if (code[p].op == "write_e") {
            manager.spill("eax");
            manager.spill("ecx");
            manager.spill("edx");
            out.emit(string("    push ") + manager.locate(code[p].rd->sv));
            out.emit(string("    push dword __fout_int"));
            out.emit(string("    call    _printf"), code[p]);
            out.emit(string("    add esp, 8\t\t;; pop stack at once."));
        }
        else if (code[p].op == "write_s") {
            manager.spill("eax");
            manager.spill("ecx");
            manager.spill("edx");
            asciis.emplace_back(make_pair(code[p].rd->sv, code[p].rs->value()));
            out.emit(string("    push dword __L") + code[p].rs->value());
            out.emit(string("    push dword __fout_string"));
            out.emit(string("    call    _printf"), code[p]);
            out.emit(string("    add esp, 8\t\t;; pop stack at once."));
        }
        else if (code[p].op == "+") {
            std::string rd = code[p].rd->sv, rs = code[p].rs->value(), rt = code[p].rt->value();
            std::string dest = manager.load(rd);
            if (code[p].rs->t == Value::TYPE::INT && code[p].rt->t == Value::TYPE::INT) {
                out.emit(string("    mov ") + dest + ", " + rs);
                out.emit(string("    add ") + dest + ", " + rt, code[p]);
            }
            else if (code[p].rs->t == Value::TYPE::INT) {
                if (rd != rt) {
                    out.emit(string("    mov ") + dest + ", " + manager.locate(rt));
                }
                out.emit(string("    add ") + dest + ", " + rs, code[p]);
            }
            else if (code[p].rt->t == Value::TYPE::INT) {
                if (rd != rs) {
                    out.emit(string("    mov ") + dest + ", " + manager.locate(rs));
                }
                out.emit(string("    add ") + dest + ", " + rt, code[p]);
            }
            else {
                if (rd != rs && rd != rt) {
                    out.emit(string("    mov ") + dest + ", " + manager.locate(rs));
                }
                if (rd == rt) {
                    out.emit(string("    add ") + dest + ", " + manager.locate(rs), code[p]);
                }
                else {
                    out.emit(string("    add ") + dest + ", " + manager.locate(rt), code[p]);
                }                
            }
        }
        else if (code[p].op == "-") {
            std::string rd = code[p].rd->sv, rs = code[p].rs->value(), rt = code[p].rt->value();
            std::string dest = manager.load(rd);
            if (rs == rt) {
                out.emit(string("    mov ") + dest + ", 0", code[p]);
            }
            else if (code[p].rs->t == Value::TYPE::INT && code[p].rt->t == Value::TYPE::INT) {
                out.emit(string("    mov ") + dest + ", " + rs);
                out.emit(string("    sub ") + dest + ", " + rt, code[p]);
            }
            else if (code[p].rs->t == Value::TYPE::INT) {
                if (rd == rt) {
                    out.emit(string("    neg ") + dest);
                    out.emit(string("    add ") + dest + ", " + rs, code[p]);
                }
                else {
                    out.emit(string("    mov ") + dest + ", " + rs);
                    out.emit(string("    sub ") + dest + ", " + manager.locate(rt), code[p]);
                }
            }
            else if (code[p].rt->t == Value::TYPE::INT) {
                if (rd != rs) {
                    out.emit(string("    mov ") + dest + ", " + manager.locate(rs));
                }
                out.emit(string("    sub ") + dest + ", " + rt, code[p]);
            }
            else {
                if (rd == rt) {
                    out.emit(string("    neg ") + dest);
                    out.emit(string("    add ") + dest + ", " + manager.locate(rs), code[p]);
                }
                else {
                    if (rd != rs) {
                        out.emit(string("    mov ") + dest + ", " + manager.locate(rs));
                    }
                    out.emit(string("    sub ") + dest + ", " + manager.locate(rt), code[p]);
                }
            }
        }
        else if (code[p].op == "*") {
            manager.spill("eax");
            manager.spill("edx");
            if (code[p].rt->t == Value::TYPE::INT) {
                out.emit(string("    mov edx, ") + code[p].rt->value());
            }
            else {
                out.emit(string("    mov edx, ") + manager.locate(code[p].rt->sv));
            }
            if (code[p].rs->t == Value::TYPE::INT) {
                out.emit(string("    mov eax, ") + code[p].rs->value());
            }
            else {
                out.emit(string("    mov eax, ") + manager.locate(code[p].rs->sv));
            }
            out.emit(string("    imul edx"), code[p]);
            manager.remap("eax", code[p].rd->sv);
        }
        else if (code[p].op == "/") {
            manager.spill("eax");
            manager.spill("edx");
            out.emit(string("    mov edx, 0"));
            std::string rt = code[p].rt->t == Value::TYPE::INT ? code[p].rt->value() : manager.locate(code[p].rt->sv);
            if (code[p].rs->t == Value::TYPE::INT) {
                out.emit(string("    mov eax, ") + code[p].rs->value());
            }
            else {
                out.emit(string("    mov eax, ") + manager.locate(code[p].rs->sv));
            }
            out.emit("    cdq");
            out.emit("    idiv " + rt, code[p]);
            manager.remap("eax", code[p].rd->sv);
        }
        else if (code[p].op == "%") {
            manager.spill("eax");
            manager.spill("edx");
            out.emit(string("    mov edx, 0"));
            std::string rt = code[p].rt->t == Value::TYPE::INT ? code[p].rt->value() : manager.locate(code[p].rt->sv);
            if (code[p].rs->t == Value::TYPE::INT) {
                out.emit(string("    mov eax, ") + code[p].rs->value());
            }
            else {
                out.emit(string("    mov eax, ") + manager.locate(code[p].rs->sv));
            }
            out.emit(string("    cdq")); // Convert double-word to quad-word
            out.emit(string("    idiv ") + rt, code[p]);
            manager.remap("edx", code[p].rd->sv);
        }
        else if (code[p].op == "label") {
            manager.spillAll();
            out.emit(code[p].rd->sv + ":");
        }
        else if (code[p].op == "cmp") {
            std::string comp;
            if (code[p].rs->t == Value::TYPE::INT && code[p].rt->t == Value::TYPE::INT ) {
                std::string t = manager.load(code[p].rd->sv);
                out.emit(string("    mov ") + t + ", " + code[p].rs->value());
                manager.release(t, true);
                comp = string("    cmp ") + t + ", " + code[p].rt->value();
            }
            else if (code[p].rs->t == Value::TYPE::INT) {
                comp = string("    cmp ") + code[p].rs->value() + ", " + manager.locate(code[p].rt->sv);
            }
            else if (code[p].rt->t == Value::TYPE::INT) {
                comp = string("    cmp ") + manager.locate(code[p].rs->sv) + ", " + code[p].rt->value();
            }
            else {
                if (manager.exist(code[p].rs->sv).length() == 0 && manager.exist(code[p].rt->sv).length() == 0) {
                    manager.load(code[p].rs->sv);
                }
                comp = string("    cmp ") + manager.locate(code[p].rs->sv) + ", " + manager.locate(code[p].rt->sv);
            }
            manager.spillAll();
            out.emit(comp, code[p]);
        }
        else if (code[p].op == "goto") {
            out.emit(string("    ") + code[p].rd->sv + " __L" + code[p].rs->value(), code[p]);
        }
        else if (code[p].op == "forbegin") {
            std::string iter = manager.load(code[p].rd->sv);
            if (code[p].rs->t == Value::TYPE::INT) {
                out.emit(string("    mov ") + iter + ", " + code[p].rs->value(), code[p]);
            }
            else {
                out.emit(string("    mov ") + iter + ", " + manager.locate(code[p].rs->sv), code[p]);
            }
        }
        else if (code[p].op == "forend") {
            manager.spillAll();
            out.emit(string("    ") + code[p].rd->sv + " __L" + code[p].rs->value(), code[p]);
        }
        else if (code[p].op == "switch") {
            std::string cond;
            if (code[p].rd->t == Value::TYPE::INT) {
                cond = manager.load(code[p].rs->sv);
                out.emit(string("    mov " + cond + ", " + code[p].rd->value()));
            }
            else {
                cond = manager.load(code[p].rd->sv);
            }
            while(code[++p].op == "case") {
                manager.spillAll();
                out.emit("    cmp " + cond + ", " + code[p].rd->value());
                out.emit("    je __L" + code[p].rs->value());
            }
            p = p - 1;
        }
        else if (code[p].op == "endswitch") {
            manager.spillAll();
            out.emit("__L" + code[p].rd->value() + ":");
        }
        else {
            out.emit("UNIMPLEMENT", code[p]);
        }
        // for next three-address-code.
        p = p + 1;
    }
    manager.dump();
    manager.release("eax", true);
    manager.spillAll();
    out.emit("    leave");
    out.emit("    ret", code[p++]);
    runtime.detag();
    dist = old;
}

void pl0_x86_gen(std::string file, std::vector<TAC> & tac) {
    // out.emit(string("    .file    \"") + file + "\"");
    // out.emit(string("    .intel_syntax noprefix"));
    // out.emit(string("    .ident   \"") + " pl0 compiler.\"");
    out.emit(string("    bits 32\t\t;; 32bit machine"));
    out.emit(string("    cpu 686\t\t;; i686 CPU"));
    out.emit(string(""));
    out.emit(string(";; external functions (from standard C library)"));
    out.emit(string("    extern _scanf"));
    out.emit(string("    extern _printf"));
    out.emit(string(""));
    out.emit(string("section .text"));
    code = tac; p = 1;
    x86_gen_callable();
    // dump all constant ascii string.
    out.emit(string(""));
    out.emit(string("section .data"));
    out.emit(string("    __fin_int:        db      \"%d\", 0x0"));
    out.emit(string("    __fin_char:       db      \"%c\", 0x0"));
    out.emit(string("    __fin_string:     db      \"%s\", 0x0"));
    out.emit(string("    __fout_int:       db      \"%d\", 0xA, 0x0"));
    out.emit(string("    __fout_char:      db      \"%c\", 0xA, 0x0"));
    out.emit(string("    __fout_string:    db      \"%s\", 0xA, 0x0"));
    out.emit(string(""));
    out.emit(string("section .data"));
    for (auto && a: asciis) {
        out.emit(string("    __L") + a.second + ":\t\tdb\t\t\"" + a.first + "\", 0x0");
    }
}




