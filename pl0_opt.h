#ifndef __PL0_OPT_H__
#define __PL0_OPT_H__

#include <vector>
#include <map>
#include <algorithm>
#include "pl0_ast.hpp"

using namespace std;

struct DAGNode {
    Value *val;
    string op;
    int lhs, rhs;
    DAGNode(): val(nullptr), op(""), lhs(-1), rhs(-1) {}
    DAGNode(Value *val): val(val), op(""), lhs(-1), rhs(-1) {}
    DAGNode(Value *val, string op, int lhs, int rhs = -1): val(val), op(op), lhs(lhs), rhs(rhs) {}
};

class BasicBlock {
public:
    const int no;
    bool canopt, is_end;
    int begin, end;
    std::vector<TAC> code;
    std::vector<int> prefix, suffix;
public:
    BasicBlock(int const no, bool const canopt): no(no), canopt(canopt), is_end(false), begin(0), end(0) {}
    void push(TAC &);
    void dump();
    size_t size();
    void setBegin(int begin);
    void setEnd(int end);
    void setPrefix(std::vector<int> &);
    void setSuffix(std::vector<int> &);
    void doCSE();
private:
    void buildDAG();
};

int pl0_block(std::vector<TAC> &, std::vector<BasicBlock> &, int p = 1);




#endif /* __PL0_OPT_H__ */

