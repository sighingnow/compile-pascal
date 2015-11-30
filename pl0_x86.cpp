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
static SimpleAllocator manager(runtime, out);

void x86_gen_program() {
    out.emit("    .text");
    out.emit("    .global _main");
    out.emit("    .def _main; .scl 2; .type 32; .endef");
    out.emit("_main:");
    out.emit("    push ebp");
    out.emit("    mov ebp, esp");
    old = dist; dist = 0;
}

std::string x86_gen_def() {
    int size;
    if (code[p].rs->sv == "integer") {
        size = code[p].rt->iv == -1 ? 4 : (code[p].rt->iv * 4);
    }
    else {
        size = code[p].rt->iv == -1 ? 4 : (code[p].rt->iv * 4); // TODO
    }
    dist -= size;
    runtime.push(LOC(code[p].rd->sv, dist));
    return string("    sub esp, ") + std::to_string(size) + " // " + code[p].str();
}

void x86_gen_param() {
    int size = 4;
    while (code[p].op == "param" || code[p].op == "paramref") {
        runtime.push(LOC(code[p].rd->sv, size, code[p].op == "paramref"));
        p = p + 1; size = size + 4;
    }
}

void x86_gen_callable() {
    vector<string> buffer;
    buffer.emplace_back("");
    old = dist; dist = 0;
    x86_gen_param();
    buffer.emplace_back(string("    .global ") + code[p].rd->sv);
    buffer.emplace_back(string("    .def ") + code[p].rd->sv + "; .scl 2; .type 32; .endef");
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
        // binary operator: alloc register, then move left oprand to this register, then do calculus.
        if (code[p].op == "=") {
            if (code[p].rs->t == Value::TYPE::INT) {
                out.emit(string("    mov " + manager.locate(code[p].rd->sv) + ", " + std::to_string(code[p].rs->iv)));
            }
            else {
                if (manager.exist(code[p].rd->sv).length() == 0 && manager.exist(code[p].rs->sv).length() == 0) {
                    out.emit("    mov " + manager.alloc(code[p].rd->sv) + ", " + manager.locate(code[p].rs->sv));
                }
                else {
                    out.emit("    mov " + manager.locate(code[p].rd->sv) + ", " + manager.locate(code[p].rs->sv));
                }
            }
        }
        else if (code[p].op == "+") {
            std::string reg = manager.alloc(code[p].rd->sv);
            if (code[p].rs->t == Value::TYPE::STR) {
                manager.load(code[p].rs->sv, reg);
                if (code[p].rt->t == Value::TYPE::INT) {
                    out.emit(string("    add " + reg + ", " + std::to_string(code[p].rt->iv)));
                }
                else {
                    out.emit(string("    add " + reg + ", " + manager.locate(code[p].rt->sv)));
                }
            }
            else {
                manager.load(code[p].rt->sv, reg);
                if (code[p].rt->t == Value::TYPE::INT) {
                    out.emit(string("    add " + reg + ", " + std::to_string(code[p].rs->iv)));
                }
                else {
                    out.emit(string("    add " + reg + ", " + manager.locate(code[p].rs->sv)));
                }
            }
        }
        else if (code[p].op == "-") {
            std::string reg = manager.alloc(code[p].rd->sv);
            if (code[p].rs->t == Value::TYPE::STR) {
                manager.load(code[p].rs->sv, reg);
                if (code[p].rt->t == Value::TYPE::INT) {
                    out.emit(string("    sub " + reg + ", " + std::to_string(code[p].rt->iv)));
                }
                else {
                    out.emit(string("    sub " + reg + ", " + manager.locate(code[p].rt->sv)));
                }
            }
            else {
                manager.load(code[p].rt->sv, reg);
                if (code[p].rs->t == Value::TYPE::INT) {
                    out.emit(string("    sub " + reg + ", " + std::to_string(code[p].rs->iv)));
                }
                else {
                    out.emit(string("    sub " + reg + ", " + manager.locate(code[p].rs->sv)));
                }
            }
        }

        // for next three-address-code.
        p = p + 1;
    }
    manager.dump();
    out.emit("    leave");
    out.emit("    ret", code[p++]);
    dist = old;
}

void pl0_x86_gen(std::string file, std::vector<TAC> & tac) {
    out.emit(string("    .file    \"") + file + "\"");
    out.emit(string("    .intel_syntax noprefix"));
    out.emit(string("    .ident   \"") + " pl0 compiler.\"");
    code = tac; p = 1;
    x86_gen_callable();
}




