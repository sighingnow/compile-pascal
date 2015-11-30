#include <pl0_regalloc.h>

RegisterAllocator::RegisterAllocator(pl0_env<LOC> & env, struct IOOut & out): env(env), out(out) {}

/***************** Simple Register Allocator (for x86 machine) *****************/

SimpleAllocator::SimpleAllocator(pl0_env<LOC> & env, struct IOOut & out): RegisterAllocator(env, out) {}

string SimpleAllocator::alloc(std::string name) {
    string res = "";
    for (int i = 0; i < N; ++i) {
        if (!used[i] || record.at(reg[i]) == name) {
            used[i] = true;
            record.emplace(reg[i], name);
            res = reg[i]; // found free register, or this variable already mapped to register.
            break;
        }
    }
    if (res == "") {
        int t = random();
        // TODO: store.
        res = reg[t];
    }
    return res;
}

void SimpleAllocator::release(std::string name) {

}

string SimpleAllocator::load(std::string name, std::string target) {
    std::string res;
    if ((res = exist(name)).length() == 0) {
        int d = env.depth(name); // TODO
        LOC loc;
        env.find(name, true, loc);
        out.emit("    mov " + target + ", DWORD PTR [ebp" + std::to_string(loc.offset) + "]");
    }
    else {
        if (target != res) {
            out.emit("    mov " + target + ", " + res);
        }
    }
    return target;
}

void SimpleAllocator::store(std::string name) {

}

std::string SimpleAllocator::locate(std::string name) {
    std::string res;
    if ((res = exist(name)).length() == 0) {
        int d = env.depth(name); // TODO
        LOC loc;
        env.find(name, true, loc);
        res = "DWORD PTR [ebp" + std::to_string(loc.offset) + "]";
    }
    return res;
}
