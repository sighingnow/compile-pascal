#include "pl0_opt.h"

void BasicBlock::push(TAC & tac) {
    this->code.emplace_back(tac);
}

void BasicBlock::dump() {
    cout << ";; +++++++++++++++++++ Basic block: ";
    if (this->no == 0) {
        cout << "header ";
        cout << "begin with: " << this->begin << " ";
        cout << "end with: " << this->end << " ";
    }
    else {
        cout << this->no << " ";
        cout << "prefix: [";
        for (auto && i: this->prefix) { cout << i << " "; }
        cout << "]  ";
        cout << "suffix: [";
        for (auto && i: this->suffix) { cout << i << " "; }
        cout << "]";
    }
    if (this->is_end) { cout << " is end "; }
    cout << endl;
    for (auto && c: this->code) {
        cout << ";; " << c.str() << endl;
    }
}

size_t BasicBlock::size() {
    return this->code.size();
}

void BasicBlock::setBegin(int begin) {
    this->begin = begin;
}

void BasicBlock::setEnd(int end) {
    this->end = end;
}

void BasicBlock::setPrefix(std::vector<int> & prefix) {
    this->prefix = prefix;
}

void BasicBlock::setSuffix(std::vector<int> & suffix) {
    this->suffix = suffix;
}

void BasicBlock::buildDAG() {
    size_t p = 1;
    std::map<Value, int> record;
    std::vector<DAGNode> G;
    int rd, rs, rt;
    for (p = 1; code[p].op != "goto" && code[p].op != "loadret" && code[p].op != "endproc"
            && code[p].op != "forbegin" && code[p].op != "forend"; ++p) {
        if (code[p].op == "*" || code[p].op == "/" || code[p].op == "%"
                || code[p].op == "+" || code[p].op == "-"
                || code[p].op == "cmp"
                || code[p].op == "=[]" || code[p].op == "[]=") {
            // find left operand.
            if (record.count(*(code[p].rs)) == 0) {
                rs = G.size();
                G.emplace_back(DAGNode(code[p].rs));
            }
            else {
                rs = record[*(code[p].rs)];
            }
            record.emplace(*(code[p].rs), rs);
            // find right operand.
            if (record.count(*(code[p].rt)) == 0) {
                rt = G.size();
                G.emplace_back(DAGNode(code[p].rd));
            }
            else {
                rt = record[*(code[p].rt)];
            }
            record.emplace(*(code[p].rt), rt);
            // find op and target.
            auto iter = std::find_if(G.begin(), G.end(), [&](DAGNode & node) {
                return code[p].op == node.op;
            });
            if (iter == G.end() || iter->lhs != rs || iter->rhs != rt) {
                rd = G.size();
                G.emplace_back(DAGNode(code[p].rd, code[p].op, rs, rt));
            }
            else {
                rd = iter - G.begin();
            }
            record.emplace(*(code[p].rd), rd);
        }
        else if (code[p].op == "=") {
            if (record.count(*(code[p].rs)) == 0) {
                rs = G.size();
                G.emplace_back(DAGNode(code[p].rs));
            }
            else {
                rs = record[*(code[p].rs)];
            }
            record.emplace(*(code[p].rs), rs);
            auto iter = std::find_if(G.begin(), G.end(), [&](DAGNode & node) {
                return code[p].op == node.op;
            });
            if (iter == G.end() || iter->lhs != rs || iter->rhs != rt) {
                rd = G.size();
                G.emplace_back(DAGNode(code[p].rd, code[p].op, rs, rt));
            }
            else {
                rd = iter - G.begin();
            }
            record.emplace(*(code[p].rd), rd);
        }
        // else if (code[p].op == "setret" || code[p].op == "callfunc")
    }
}

void BasicBlock::doCSE() {
    if (this->no == 0) { return; }
    
}

static int pl0_block_helper(std::vector<TAC> & code, std::vector<BasicBlock> & bbs, int p,
        std::map<int, std::vector<int>> & pres, std::map<int, std::vector<int>> & sufs)
{
    BasicBlock header(0, false);
    while (code[p].op == "param" || code[p].op == "paramref"
            || code[p].op == "function" || code[p].op == "procedure") {
        header.push(code[p++]);
    }
    while (code[p].op == "def" || code[p].op == "allocret") {
        header.push(code[p++]);
    }
    int hidx = bbs.size();
    bbs.emplace_back(header);
    while (code[p].op == "param" || code[p].op == "paramref"
            || code[p].op == "function" || code[p].op == "procedure") {
        p = pl0_block_helper(code, bbs, p, pres, sufs);
    }
    cout << ";; setBegin " << code[p].str() << endl;
    bbs[hidx].setBegin(code[p].rd->iv);

    while (code[p].op != "endproc" && code[p].op != "endfunc") {
        BasicBlock body(code[p].rd->iv, true);
        body.push(code[p++]);
        while (code[p].op != "label" && code[p].op != "endproc" && code[p].op != "endfunc") {
            if (code[p].op == "goto" || code[p].op == "forend") {
                sufs[body.no].emplace_back(code[p].rs->iv);
                pres[code[p].rs->iv].emplace_back(body.no);
            }
            if (code[p].op == "cmp" || code[p].op == "forbegin") {
                sufs[body.no].emplace_back(code[p].rd->iv);
                pres[code[p].rd->iv].emplace_back(body.no);
            }
            body.push(code[p++]);
        }
        if (code[p].op != "label") {
            body.push(code[p]);
        }
        bbs.emplace_back(body);
    }
    bbs[hidx].setEnd(bbs.back().no);
    bbs.back().is_end = true;
    return (++p);
}

int pl0_block(std::vector<TAC> & code, std::vector<BasicBlock> & bbs, int p) {
    std::map<int, std::vector<int>> pres, sufs;
    pl0_block_helper(code, bbs, p, pres, sufs);
    for (auto && bb: bbs) {
        bb.setPrefix(pres[bb.no]);
        bb.setSuffix(sufs[bb.no]);
    }
    return bbs.size();
}

// Common subexpression elimination (via DAG)
void CSEPass(std::vector<BasicBlock> & bbs) {
    for (auto && bb: bbs) {
        bb.doCSE();
    }
}

static void GenKillHelper(std::vector<BasicBlock> & bbs, int p = 0) {
    // int hidx = p;
    // while (bbs[p].no == 0) {
    //     p = GenKillHelper(bbs, p);
    // }
    // std::map<int, std::set<string>> gen, kill;
    // while (!bbs[p].is_end) {
    //     for (auto && c: bbs[p].code) {
    //         if (code[p].op == "*" || code[p].op == "/" || code[p].op == "%"
    //             || code[p].op == "+" || code[p].op == "-"
    //             || code[p].op == "cmp"
    //             || code[p].op == "=[]" || code[p].op == "[]=") {

    //         }
    //     }
    // }
}

void ReachDefPass(std::vector<BasicBlock> & bbs) {

}

// Data flow analysis
// void DFAPass()
// Constant folding and constant propagation
// void CFPPass()

