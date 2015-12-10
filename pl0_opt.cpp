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
    size_t p = this->s;
    std::map<Value, int> record;
    int rd, rs, rt, t;
    for (p = 1; code[p].op != "goto" && code[p].op != "call"
            && code[p].op != "loadret" && code[p].op != "exit" && code[p].op != "endproc"; ++p)
    {
        if (code[p].op == "*" || code[p].op == "/" || code[p].op == "%"
                || code[p].op == "+" || code[p].op == "-"
                || code[p].op == "cmp"
                || code[p].op == "=[]" || code[p].op == "[]=") {
            // find left operand.
            if (record.count(*(code[p].rs)) == 0) {
                rs = G.size();
                G.emplace_back(DAGNode(rs, code[p].rs));
                record.emplace(*(code[p].rs), rs);
            }
            else {
                rs = record[*(code[p].rs)];
            }
            // find right operand.
            if (record.count(*(code[p].rt)) == 0) {
                rt = G.size();
                G.emplace_back(DAGNode(rt, code[p].rt));
                record.emplace(*(code[p].rt), rt);
            }
            else {
                rt = record[*(code[p].rt)];
            }
            // find op and target.
            auto iter = std::find_if(G.begin(), G.end(), [&](DAGNode & node) {
                return code[p].op == node.op && rs == node.lhs && rt == node.rhs;
            });
            if (iter == G.end()) {
                rd = G.size();
                G.emplace_back(DAGNode(rd, code[p].rd, code[p].op, rs, rt));
            }
            else {
                rd = iter - G.begin();
                G[rd].addAttr(code[p].rd);
            }
            G[rs].moreFa(); G[rt].moreFa();
            G[rd].addSon(rs); G[rd].addSon(rt);
            record[*(code[p].rd)] = rd; // insert or assign (replace)
        }
        else if (code[p].op == "=") {
            if (record.count(*(code[p].rs)) == 0) {
                rs = G.size();
                G.emplace_back(DAGNode(rs, code[p].rs));
                record.emplace(*(code[p].rs), rs);
            }
            else {
                rs = record[*(code[p].rs)];
            }
            rd = rs;
            G[rd].addAttr(code[p].rd);
            record[*(code[p].rd)] = rd;
        }
        else if (code[p].op == "write_s") {
            IOBuf.emplace_back(make_pair(p, code[p]));
        }
        else if (code[p].op == "write_e") {
            if (record.count(*(code[p].rd)) == 0) {
                rd = G.size();
                G.emplace_back(DAGNode(rd, code[p].rd));
                record.emplace(*(code[p].rd), rd);
            }
            else {
                rd = record[*(code[p].rd)];
            }
            t = G.size();
            G.emplace_back(DAGNode(t, new Value("~write_e", "cmd"), "write_e", rd, p));
            G[rd].moreFa(); G[t].addSon(rd);
        }
        else if (code[p].op == "read") {
            if (record.count(*(code[p].rd)) == 0) {
                rd = G.size();
                G.emplace_back(DAGNode(rd, code[p].rd));
                record.emplace(*(code[p].rd), rd);
            }
            else {
                rd = record[*(code[p].rd)];
            }
            t = G.size();
            G.emplace_back(DAGNode(t, code[p].rd, "read", rd, p));
            G[rd].moreFa(); G[t].addSon(rd);
            record[*(code[p].rd)] = t;
        }
    }
    this->t = p;
}

void BasicBlock::solveDAG() {
    for (auto && node: G) {
        node.finalizeAttr();
    }
    for (auto && g: G) {
        cout << ";; " << g.str() << endl;
    }
    std::vector<int> Q;
    while (Q.size() < G.size()) {
        int sel = 0, p = -1;
        for (p = G.size() - 1; p >= 0; --p) {
            if (!G[p].used && G[p].noFa()) { sel = p; break; }
        }
        Q.emplace(Q.begin(), sel); G[sel].used = true;
        for (auto && s: G[sel].sons) {
            G[s].lessFa();
        }
        int l = G[sel].lhs;
        while (l != -1 && !G[l].used && G[l].noFa()) {
            Q.emplace(Q.begin(), l); G[l].used = true;
            for (auto && s: G[l].sons) {
                G[s].lessFa();
            }
            l = G[l].lhs;
        }
    }
    std::vector<TAC> irs;
    for (int i = 0; i < this->s; ++i) {
        irs.emplace_back(this->code[i]);
    }
    for (auto && nno: Q) {
        if (G[nno].lhs != -1 || G[nno].rhs != -1) {
            if (G[nno].op == "=") {
                irs.emplace_back(TAC(G[nno].op, G[nno].attr[0], G[G[nno].lhs].attr[0]));
            }
            else if (G[nno].op == "read") {
                while (!this->IOBuf.empty()) {
                    if (this->IOBuf[0].first < G[nno].rhs) {
                        irs.emplace_back(this->IOBuf[0].second);
                        this->IOBuf.erase(IOBuf.begin());
                    }
                    else {
                        break;
                    }
                }
                irs.emplace_back(TAC("read", G[G[nno].lhs].attr[0]));
            }
            else if (G[nno].op == "write_e") {
                while (!this->IOBuf.empty()) {
                    if (this->IOBuf[0].first < G[nno].rhs) {
                        irs.emplace_back(this->IOBuf[0].second);
                        this->IOBuf.erase(IOBuf.begin());
                    }
                    else {
                        break;
                    }
                }
                irs.emplace_back(TAC("write_e", G[G[nno].lhs].attr[0]));
            }
            else {
                irs.emplace_back(TAC(G[nno].op, G[nno].attr[0], G[G[nno].lhs].attr[0], G[G[nno].rhs].attr[0]));
            }
        }
        else {
            // DO NOTHING.
        }
        if (G[nno].attr.size() > 1) {
            for (size_t i = 1; i < G[nno].attr.size(); ++i) {
                irs.emplace_back(TAC("=", G[nno].attr[i], G[nno].attr[0]));
            }
        }
    }
    while (!this->IOBuf.empty()) {
        irs.emplace_back(this->IOBuf[0].second);
        this->IOBuf.erase(IOBuf.begin());
    }
    for (size_t i = t; i < this->code.size(); ++i) {
        irs.emplace_back(this->code[i]);
    }
    this->code.swap(irs);
}

// Common subexpression elimination (via DAG)
void BasicBlock::DAGPass() {
    if (this->no == 0) { return; }
    this->buildDAG();
    this->solveDAG();
}

static int pl0_block_helper(std::vector<TAC> & code, std::vector<BasicBlock> & bbs, int p,
        std::map<int, std::vector<int>> & pres, std::map<int, std::vector<int>> & sufs)
{
    BasicBlock header(0, false);
    while (code[p].op == "param" || code[p].op == "paramref") {
        header.push(code[p++]);
    }
    header.push(code[p++]); // function or procedure.
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
        while (code[p].op != "goto" && code[p].op != "call"
                && code[p].op != "endproc" && code[p].op != "endfunc") {
            if (code[p].op == "cmp") {
                sufs[body.no].emplace_back(code[p].rd->iv);
                pres[code[p].rd->iv].emplace_back(body.no);
            }
            if (code[p].op == "def") {
                // add temporary variable to symbol table.
                bbs[hidx].push(code[p++]);
            }
            else {
                body.push(code[p++]);
            }
        }
        if (code[p].op == "goto") {
            // goto label
            sufs[body.no].emplace_back(code[p].rs->iv);
            pres[code[p].rs->iv].emplace_back(body.no);
            body.push(code[p++]);
        }
        else if (code[p].op == "call") {
            // call ... -> ...
            // label: 
            body.push(code[p++]);
            sufs[body.no].emplace_back(code[p].rd->iv);
            pres[code[p].rd->iv].emplace_back(body.no);
        }
        else {
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

