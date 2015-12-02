#include <pl0_regalloc.h>

RegisterAllocator::RegisterAllocator(pl0_env<LOC> & env, struct IOOut & out, int & dist): env(env), out(out), dist(dist) {}

/***************** Simple Register Allocator (for x86 machine) *****************/

SimpleAllocator::SimpleAllocator(pl0_env<LOC> & env, struct IOOut & out, int & dist): RegisterAllocator(env, out, dist) {
    for (auto && r: regs) {
        this->used.emplace(r, false);
    }
}

string SimpleAllocator::alloc(std::string name) {
    string res;
    if ((res = exist(name)).length() != 0) {
        return res;
    }
    for (auto && r: regs) {
        if (!used[r] || record[r] == name) {
            this->record[r] = name;
            used[r] = true;
            return r; // found free register, or this variable already mapped to register.
        }
    }
    std::string r = regs[this->random()];
    this->spill(r);
    this->used[r] = true;
    this->record[r] = name;
    return r;
}

void SimpleAllocator::remap(std::string reg, std::string dst) {
    this->record[reg] = dst;
}

void SimpleAllocator::release(std::string name, bool is_reg) {
    if (is_reg) {
        this->used[name] = false;
        this->record.erase(name);
    }
}

string SimpleAllocator::load(std::string name, std::string target) {
    std::string res;
    if ((res = exist(name)).length() == 0) {
        out.emit("    mov " + target + ", dword " + this->addr(name));
    }
    else {
        if (target != res) {
            out.emit("    mov " + target + ", " + res);
        }
        else {
            // DO NOTHING.
        }
    }
    return target;
}

void SimpleAllocator::spill(std::string reg) {
    // this->dump();
    this->store(this->record[reg]);
    this->used[reg] = false;
    this->record.erase(reg);
    // this->dump();
}

void SimpleAllocator::store(std::string name) {
    LOC loc;
    // alloc space on runtime stack.
    if (this->env.find(name, false, loc) == false) {
        this->dist = this->dist - 4;
        env.push(LOC(name, dist));
        out.emit(string("    sub esp, 4        ;; store temporary variable ") + name + " on runtime stack.");
    }
    // store value from register to memory.
    for (auto && p: this->record) {
        if (p.second == name) {
            if (loc.name.length() == 0) {
                out.emit(string("    mov dword [esp], ") + p.first);
            }
            else {
                out.emit(string("    mov dword [ebp") + loc.offset + "], " + p.first);
            }
        }
    }
}

std::string SimpleAllocator::locate(std::string name) {
    std::string res;
    if ((res = exist(name)).length() == 0) {
        if (name == "~ret") {
            res = "eax";
        }
        else {
            LOC loc;
            env.find(name, true, loc);
            res = "dword " + this->addr(name);
        }
    }
    return res;
}

std::string SimpleAllocator::addr(std::string name) {
    int d = env.depth() - env.depth(name);
    LOC loc;
    env.find(name, true, loc);
    if (d == 0) {
        return "[ebp" + loc.offset + "]";
    }
    else {
        out.emit("    mov eax, dword [esp]");
        d = d - 1;
        while(d--) {
            out.emit("    lea eax, [eax]");
        }
        return "[eax" + loc.offset + "]";
    }
}

