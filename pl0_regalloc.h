#include <map>
#include <string>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include "pl0_ast.hpp"

using namespace std;

class RegisterAllocator
{
protected:
    static const int N = 6;
    pl0_env<LOC> & env;
    struct IOOut & out;
    // %eax, %ecx, %edx, %ebx, %esi, %edi
    std::string reg[N] = {"eax", "ecx", "edx", "ebx", "esi", "edi"};
    bool used[N] = {false, false, false, false, false, false};
    std::map<std::string, std::string> record;
public:
    RegisterAllocator(pl0_env<LOC> &, struct IOOut &);
    virtual string alloc(std::string) = 0;
    virtual void release(std::string) = 0;
    virtual string load(std::string, std::string) = 0;
    virtual void store(std::string) = 0;
    virtual string locate(std::string) = 0;
    int random() {
        std::srand(std::time(0));
        return std::rand() % N;
    }
    string exist(std::string name) {
        for (auto && x: record) {
            if (x.second == name) {
                return x.first;
            }
        }
        return "";
    }
    void dump() {
        cout << "------------ register mapping ----------------" << endl;
        for (auto && x: record) {
            cout << x.first << " : " << x.second << endl;
        }
        cout << "----------------------------------------------" << endl;
    }
};

class SimpleAllocator: public RegisterAllocator {
public:
    SimpleAllocator(pl0_env<LOC> &, struct IOOut &);
    string alloc(std::string);
    void release(std::string);
    string load(std::string, std::string);
    void store(std::string);
    string locate(std::string);
};





