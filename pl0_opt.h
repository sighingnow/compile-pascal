#ifndef __PL0_OPT_H__
#define __PL0_OPT_H__

#include <vector>
#include <map>
#include <algorithm>
#include "pl0_ast.hpp"
#include "patch.hpp"

using namespace std;

struct DAGNode {
    int no, fa;
    bool used;
    std::vector<Value *> attr;
    string op;
    int lhs, rhs;
    std::vector<int> sons;
    DAGNode(): no(-1), fa(0), used(false), op(""), lhs(-1), rhs(-1) {}
    DAGNode(int no, Value *val): no(no), fa(0), used(false), op(""), lhs(-1), rhs(-1) { attr.emplace_back(val); }
    DAGNode(int no, Value *val, string op, int lhs, int rhs = -1): no(no), fa(0), used(false), op(op), lhs(lhs), rhs(rhs) { attr.emplace_back(val); }
    string const str() const {
        string ans = string("Node ") + to_string(no) + string(": ");
        ans += op + " ";
        ans += to_string(lhs) + " " + to_string(rhs) + " -> [";
        for (auto && v: attr) {
            ans += v->str() + ", ";
        }
        ans += "], fa: " + to_string(fa);
        return ans;
    }
    void addAttr(Value *val) {
        this->attr.emplace_back(val);
    }
    void finalizeAttr() {
        auto iter = std::find_if(attr.begin(), attr.end(), [](Value *val) {
            return val->t == Value::TYPE::INT || val->sv[0] != '~' || (val->sv.length() >= 2 && val->sv[1] == '~');
        });
        if (iter == attr.end()) {
            attr.resize(1);
        }
        else {
            for (size_t i = 0; i < attr.size(); ++i) {
                if (attr[i]->sv[0] == '~') {
                    attr.erase(attr.begin() + i--);
                }
            }
        }
    }
    void addSon(int s) { this->sons.emplace_back(s); }
    void moreFa() { this->fa++; }
    void lessFa() { this->fa--; }
    bool noFa() const { return this->fa == 0; }
};

class BasicBlock {
public:
    const int no;
    bool canopt, is_end;
    int begin, end, s, t;
    std::vector<TAC> code;
    std::vector<int> prefix, suffix;
    std::vector<DAGNode> G;
    std::vector<std::pair<int, TAC>> IOBuf;
public:
    BasicBlock(int const no, bool const canopt): no(no), canopt(canopt), is_end(false), begin(0), end(0), s(1), t(-1) {}
    void push(TAC &);
    void dump();
    size_t size();
    void setBegin(int begin);
    void setEnd(int end);
    void setPrefix(std::vector<int> &);
    void setSuffix(std::vector<int> &);
    void DAGPass();
private:
    void buildDAG();
    void solveDAG();
};

int pl0_block(std::vector<TAC> &, std::vector<BasicBlock> &, int p = 1);




#endif /* __PL0_OPT_H__ */

