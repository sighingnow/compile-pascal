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
    bool used, leaf;
    Value *item;
    std::vector<Value *> items;
    string op;
    int lhs, rhs;
    DAGNode(int no, Value *val): no(no), fa(0), used(false), leaf(true), item(val), op(""), lhs(-1), rhs(-1) {
        // ...
    }
    DAGNode(int no, std::string op, Value *val, int lhs, int rhs): no(no), fa(0), leaf(false), item(val), op(op), lhs(lhs), rhs(rhs) {
        // ...
    }
    string const str() const {
        string ans = string("Node ") + to_string(no) + string(", fa~") + to_string(fa) + " : ";
        ans += op + " ";
        ans += to_string(lhs) + " " + to_string(rhs) + " -> [" + item->str() + ", ";
        for (auto && v: items) {
            ans += v->str() + ", ";
        }
        ans += "]";
        return ans;
    }
    void addItem(Value *val) {
        this->items.emplace_back(val);
    }
    void moreFa() { this->fa++; }
    void lessFa() { this->fa--; }
    bool ready() const { return this->fa == 0; }
};

class BasicBlock {
public:
    const int no;
    bool canopt, is_end;
    int begin, end, s, t;
    std::vector<TAC> code;
    std::vector<int> prefix, suffix;
    std::vector<DAGNode> G;
    std::map<Value, int> record;
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
    void addNode();
    int findNode(Value *);
    int findNode(std::string op, Value *val, int lhs, int rhs);
    void trans(std::vector<TAC> & irs, int nno);
    void releaseLeaf(std::vector<TAC> & irs, Value *item);
    void setMap(Value *, int);
    void solveDAG();
};

int pl0_block(std::vector<TAC> &, std::vector<BasicBlock> &, int p = 1);




#endif /* __PL0_OPT_H__ */

