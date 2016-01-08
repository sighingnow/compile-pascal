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
    int rd, rs, rt, t;
    for (p = 1; code[p].op != "cmp" && code[p].op != "goto" && code[p].op != "call"
            && code[p].op != "loadret" && code[p].op != "exit" && code[p].op != "endproc"; ++p)
    {
        if (code[p].op == "*" || code[p].op == "/" || code[p].op == "%"
                || code[p].op == "+" || code[p].op == "-"
                || code[p].op == "cmp"
                || code[p].op == "=[]" || code[p].op == "[]=") {
            rs = findNode(code[p].rs);
            setMap(code[p].rs, rs);
            rt = findNode(code[p].rt);
            setMap(code[p].rt, rt);
            rd = findNode(code[p].op, code[p].rd, rs, rt);
            setMap(code[p].rd, rd);
        }
        else if (code[p].op == "=") {
            // rs = findNode(code[p].rs);
            // setMap(code[p].rs, rs);
            // setMap(code[p].rd, rs);
            code[p].rt = new Value(0, "integer");
            rs = findNode(code[p].rs);
            setMap(code[p].rs, rs);
            rt = findNode(code[p].rt);
            setMap(code[p].rt, rt);
            rd = findNode(code[p].op, code[p].rd, rs, rt);
            setMap(code[p].rd, rd);
        }
        else if (code[p].op == "write_s") {
            IOBuf.emplace_back(make_pair(p, code[p]));
        }
        else if (code[p].op == "write_e") {
            rd = findNode(code[p].rd);
            t = G.size();
            G.emplace_back(DAGNode(t, "write_e", new Value("~write_e", "cmd"), rd, p));
            G[rd].moreFa();
        }
        else if (code[p].op == "read") {
            rd = findNode(code[p].rd);
            t = G.size();
            G.emplace_back(DAGNode(t, "read", code[p].rd, rd, p));
            G[rd].moreFa();
            setMap(code[p].rd, t);
        }
    }
    this->t = p;
}

int BasicBlock::findNode(Value *val) {
    int k = record.count(*val);
    if (k == 0) {
        k = G.size();
        G.emplace_back(DAGNode(k, val));
        return k;
    }
    else {
        return record[*val];
    }
}

int BasicBlock::findNode(std::string op, Value *val, int rs, int rt) {
    // find op and target.
    auto iter = std::find_if(G.begin(), G.end(), [&](DAGNode & node) {
        return op == node.op && rs == node.lhs && rt == node.rhs;
    });
    if (iter != G.end()) {
        return iter - G.begin();
    }
    else {
        int k = G.size();
        G.emplace_back(DAGNode(k, op, val, rs, rt));
        G[rs].moreFa(); G[rt].moreFa();
        return k;
    }
}

void BasicBlock::setMap(Value *val, int node) {
    auto iter = record.find(*val);
    if (iter == record.end()) {
        record[*val] = node;
        G[node].addItem(val);
    }
    else if (iter->second == node) {
        return;
    }
    else {
        auto it = G[iter->second].items.begin();
        while (it != G[iter->second].items.end()) {
            if (*(*it) == *val) {
                it = G[iter->second].items.erase(it);
            }
            else {
                it++;
            }
        }
        record[*val] = node;
        G[node].addItem(val);
    }
}

void BasicBlock::solveDAG() {
    std::vector<TAC> irs;
    for (int i = 0; i < this->s; ++i) {
        irs.emplace_back(this->code[i]);
    }
    std::vector<int> stack;
    std::vector<DAGNode> t_nodes = G;

    while (!t_nodes.empty()) {
        auto iter = t_nodes.rbegin();
        bool flag = true;
        while (iter != t_nodes.rend()) {
            DAGNode & node = *iter;
            if (node.ready()) {
                stack.push_back(node.no);
                if (node.lhs != -1) {
                    t_nodes[node.lhs].lessFa();
                }
                if (node.op != "write_e" && node.op != "read" && node.rhs != -1) {
                    t_nodes[node.rhs].lessFa();
                }
                t_nodes.erase((++iter).base());
                flag = false;
                break;
            }
            else {
                iter++;
            }
        }
        if (flag) {
            std::cout << ";; DAG graph export warning: Circle found." << std::endl;
            break;
        }
    }
    auto r_iter = stack.rbegin();
    while (r_iter != stack.rend()) {
        int node = *r_iter;
        releaseLeaf(irs, G[node].item);
        r_iter++;
    }

    r_iter = stack.rbegin();
    while (r_iter != stack.rend()) {
        int node = *r_iter;
        if (!G[node].leaf) {
            this->trans(irs, node);
        }
        auto iter = G[node].items.begin();
        while (iter != G[node].items.end()) {
            if (*iter != G[node].item && (*iter)->t != Value::TYPE::IMM) {
                irs.push_back(TAC("=", *iter, G[node].item));
            }
            iter++;
        }
        r_iter++;
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

void BasicBlock::trans(std::vector<TAC> & irs, int nno) {
    if (G[nno].op == "=") {
        irs.emplace_back(TAC(G[nno].op, G[nno].item, G[G[nno].lhs].item));
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
        irs.emplace_back(TAC("read", G[G[nno].lhs].item));
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
        irs.emplace_back(TAC("write_e", G[G[nno].lhs].item));
    }
    else {
        irs.emplace_back(TAC(G[nno].op, G[nno].item, G[G[nno].lhs].item, G[G[nno].rhs].item));
    }
}

void BasicBlock::releaseLeaf(std::vector<TAC> & irs, Value *item) {
    auto iter = G.begin();
    while (iter != G.end()) {
        DAGNode & node = *iter; // reference.
        if (node.leaf && record[*item] != node.no) {
            if (node.item == item && !node.items.empty()) {
                auto i_iter = node.items.begin();
                node.item = *i_iter;
                node.items.erase(i_iter);
                releaseLeaf(irs, node.item);
                if (node.item->str() != item->str()) {
                    irs.push_back(TAC("=", node.item, item));
                }
            }
        }
        iter++;
    }
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

