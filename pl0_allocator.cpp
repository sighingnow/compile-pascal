#include <vector>
#include <iostream>
#include "pl0_ast.hpp"
#include "pl0_x86.h"
#include "patch.hpp"
using namespace std;

RegisterAllocator::RegisterAllocator(pl0_env<LOC> & env, struct IOOut & out, int & dist): env(env), out(out), dist(dist) {}

/***************** Simple Register Allocator (for x86 machine) *****************/

SimpleAllocator::SimpleAllocator(pl0_env<LOC> & env, struct IOOut & out, int & dist): RegisterAllocator(env, out, dist) {
    for (auto && r: regs) {
        this->used.emplace(r, false);
    }
}

string SimpleAllocator::alloc(std::string name, bool is_ref) {
    string res;
    if ((res = exist(name)).length() != 0) {
        return res;
    }
    LOC loc;
    for (auto && r: regs) {
        if (!used[r] || record[r] == name) {
            if (is_ref && r == "eax") {
                continue; // don't map eax to a variable with reference type.
            }
            used[r] = true;
            this->record[r] = name;
            return r; // found free register, or this variable already mapped to register.
        }
    }
    std::string r = regs[this->random()];
    if (is_ref && r == "eax") {
        r = "ebx";  // don't map eax to a variable with reference type.
    }
    this->spill(r);
    this->used[r] = true;
    this->record[r] = name;
    return r;
}

void SimpleAllocator::remap(std::string reg, std::string dst) {
    this->spill(reg);
    this->used[reg] = true;
    this->record[reg] = dst;
}

void SimpleAllocator::release(std::string name, bool is_reg) {
    if (is_reg) {
        // release register.
        if (this->used[name]) {
            this->used[name] = false;
            this->record.erase(name);
        }
    }
    else {
        // release variable.
        for (auto && p: this->record) {
            if (p.second == name) {
                this->used[p.first] = false;
                this->record.erase(p.first);
                break;
            }
        }
    }
}

string SimpleAllocator::load(std::string name, std::string target) {
    std::string now = this->exist(name);
    if (now != target) {
        this->remap(name, target);
        if (now.length() == 0) {
            if (env.find(name, true)) {
                out.emit("    mov " + target + ", dword " + this->addr(name));
            }
            else {
                // DO NOTHING, JUST MAINTAIN MAPPING.
            }
        }
        else {
            out.emit("    mov " + target + ", " + now);
            this->release(now, true);
        }
    }
    return target;
}

string SimpleAllocator::load(std::string name) {
    std::string now, target;
    if ((now = exist(name)).length() != 0) {
        return now;
    }
    else {
        LOC loc;
        if (env.find(name, true, loc)) {
            if (loc.is_ref) {
                target = this->alloc(name, true);
            }
            else {
                target = this->alloc(name);
            }
            out.emit("    mov " + target + ", dword " + this->addr(name));
        }
        else {
            target = this->alloc(name);
        }
        return target;
    }
}

void SimpleAllocator::spill(std::string reg) {
    if (this->used[reg]) {
        this->store(this->record[reg]);
        this->release(reg, true);
    }
    else {
        // DO NOTHING
    }
}

void SimpleAllocator::spillAll() {
    for (auto && r: this->regs) {
        this->spill(r);
    }
}

void SimpleAllocator::store(std::string name) {
    LOC loc;
    if (this->exist(name).length() == 0) { return; }
    // alloc space on runtime stack.
    if (this->env.find(name, true, loc) == false) {
        this->dist = this->dist - 4;
        env.push(LOC(name, dist));
        out.emit(string("    sub esp, 4\t\t;; store temporary variable ") + name + " on runtime stack.");
    }
    // store value from register to memory.
    for (auto && p: this->record) {
        if (p.second == name) {
            if (loc.name.length() == 0) {
                // temporary implicit variable.
                out.emit(string("    mov dword [esp], ") + p.first);
            }
            else {
                // non-temporary explicit variable.
                out.emit(string("    mov dword ") + this->addr(loc.name) + ", " + p.first);
            }
            this->release(p.first, true); break;
        }
    }
}

std::string SimpleAllocator::locate(std::string name) {
    std::string res;
    if ((res = exist(name)).length() == 0) {
        LOC loc;
        env.find(name, true, loc);
        res = "dword " + this->addr(name);
    }
    return res;
}

std::string SimpleAllocator::addr(std::string name) {
    int d = env.depth(name);
    LOC loc;
    env.find(name, true, loc);
    if (d == env.depth()) {
        if (loc.is_ref) {
            this->spill("eax");
            out.emit("    mov eax, dword [ebp" + loc.offset +"]");
            return "[eax]";
        }
        else {
            return "[ebp" + loc.offset + "]";
        }
    }
    else {
        this->spill("eax");
        out.emit("    mov eax, dword [ebp-" + to_string(d*4) + "]");
        if (loc.is_ref) {
            out.emit("    mov eax, dword [eax" + loc.offset + "]");
        }
        return "[eax" + loc.offset + "]";
    }
}


