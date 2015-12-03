/**
 * TAC (Three Address Code, the intermedia reperesentation format) generators.
 */

#ifndef __PL0_TAC_GEN_HPP__
#define __PL0_TAC_GEN_HPP__

#include <vector>
#include <string>
#include "pl0_ast.hpp"

// #define __func__ __func__ << endl
#define __func__ ""

bool pl0_tac_program(pl0_ast_program const *program);
void pl0_tac_prog(pl0_ast_prog const *prog);
void pl0_tac_const_stmt(pl0_ast_const_stmt const *stmts);
void pl0_tac_var_stmt(pl0_ast_var_stmt const *stmts);
void pl0_tac_procedure_stmt(pl0_ast_procedure_stmt const *stmts);
void pl0_tac_function_stmt(pl0_ast_function_stmt const *stmts);
vector<string> pl0_tac_procedure_header(pl0_ast_procedure_header const *header);
vector<string> pl0_tac_function_header(pl0_ast_function_header const *header);
vector<string> pl0_tac_param(pl0_ast_param_list const *param);
void pl0_tac_compound_stmt(pl0_ast_compound_stmt const *stmt);
void pl0_tac_stmt(pl0_ast_stmt const *stmt);
void pl0_tac_assign_stmt(pl0_ast_assign_stmt const *stmt);
void pl0_tac_call_proc(pl0_ast_call_proc const *stmt);
void pl0_tac_for_stmt(pl0_ast_for_stmt const *stmt);
void pl0_tac_cond_stmt(pl0_ast_cond_stmt const *stmt);
void pl0_tac_case_stmt(pl0_ast_case_stmt const *stmt);
void pl0_tac_read_stmt(pl0_ast_read_stmt const *stmt);
void pl0_tac_write_stmt(pl0_ast_write_stmt const *stmt);
void pl0_tac_null_stmt(pl0_ast_null_stmt const *stmt);
pair<Value *, string> pl0_tac_expr(pl0_ast_expression const *expr);
void pl0_tac_condition(pl0_ast_condtion const *cond);
pair<Value *, string> pl0_tac_term(pl0_ast_term const *term);
pair<Value *, string> pl0_tac_factor(pl0_ast_factor const * factor);
pair<Value *, string> pl0_tac_call_func(pl0_ast_call_func const *stmt);

extern struct IRBuilder irb;
static bool ready = false;

static bool status = true;
static string ERROR[] = {

};

static std::vector<std::string> scope;
static std::string scope_name() {
    string name = "";
    for (auto && s: scope) {
        name = name + "_" + s;
    }
    return name;
}
static pl0_env<struct variable> vartb;
static pl0_env<struct constant> valtb;
static pl0_env<struct proc> proctb;
static pl0_env<struct func> functb;


void pl0_ast_error(pair<int, int> loc, string msg) {
    status = false;
    cout << loc.first << ":" << loc.second << " " << "ERRRO: " << msg << endl;
}

bool pl0_tac_program(pl0_ast_program const *program) {
    cout << __func__;
    irb.emit("program", "");
    irb.emit("procedure", "_main");
    pl0_tac_prog(program->program);
    irb.emit("exit", new Value(0)); // main function: exit with 0.
    irb.emit("endproc", "_main");
    irb.emit("endprogram", "");
    return status;
}

void pl0_tac_prog(pl0_ast_prog const *prog) {
    cout << __func__;
    // set up new flag.
    valtb.tag(); vartb.tag();
    proctb.tag(); functb.tag();
    pl0_tac_const_stmt(prog->consts);
    pl0_tac_var_stmt(prog->vars);
    for (auto && e: prog->exes) {
        switch (e->t) {
            case pl0_ast_executable::type_t::PROCEDURE:
                pl0_tac_procedure_stmt(e->proc); break;
            case pl0_ast_executable::type_t::FUNCTION:
                pl0_tac_function_stmt(e->func); break;
            default: cout << "ERROR" << endl;
        }
    }
    pl0_tac_compound_stmt(prog->stmts);
    // pop stack.
    valtb.detag(); vartb.detag();
    proctb.detag(), functb.detag();
}

void pl0_tac_const_stmt(pl0_ast_const_stmt const *stmts) {
    cout << __func__;
    if (!stmts) { return; }
    for (auto && def: stmts->stmt) {
        string id = def->id->id;
        // validate.
        if (!scope.empty() && id == scope.back()) {
            pl0_ast_error(def->id->loc, string("duplicate identifier ") + "\"" + id + "\"");
        }
        else if (valtb.find(id, false) || vartb.find(id, false)) {
            pl0_ast_error(def->id->loc, string("redefinition of ") + "\"" + id + "\"");
        }
        else {
            valtb.push(constant(id, def->val->val)); // update symbol table.
        }
    }
    cout << __func__;
}

void pl0_tac_var_stmt(pl0_ast_var_stmt const *stmts) {
    cout << __func__;
    if (!stmts) { return; }
    for (auto && s: stmts->stmt) {
        for (auto && var: s->ids) {
            // validate.
            if (!scope.empty() && var->id == scope.back()) {
                pl0_ast_error(var->loc, string("duplicate identifier ") + "\"" + var->id + "\"");
            }
            else if (valtb.find(var->id, false) || vartb.find(var->id, false)) {
                pl0_ast_error(var->loc, string("redefinition of ") + "\"" + var->id + "\"");
            }
            else {
                string t = s->type->type->type + (s->type->len == -1 ? "" : "array");
                vartb.push(variable(var->id, t, s->type->len)); // update symbol table.
                irb.emit("def", new Value(var->id), new Value(t), new Value(s->type->len));
            }
        }
    }
}

void pl0_tac_procedure_stmt(pl0_ast_procedure_stmt const *stmts) {
    cout << __func__;
    for (auto && p: stmts->procs) {
        // validate.
        string pid = p.first->id->id;
        if (valtb.find(pid, false) || vartb.find(pid, false) || proctb.find(pid, false) || functb.find(pid, false)
                || (!scope.empty() && pid == scope.back())) {
            pl0_ast_error(p.first->id->loc, string("overloaded identifier ") + "\"" + pid + "\"" + " isn't a function");
        }
        else {
            scope.emplace_back(pid); // update global scope.
            vector<string> proctype = pl0_tac_procedure_header(p.first);
            irb.emit("procedure", scope_name());
            proctb.push(proc(scope_name(), proctype)); // update symbol table.
            proctb.tag(); functb.tag();
            pl0_tac_prog(p.second);
            irb.emit("endproc", scope_name());
            // end of current scope.
            scope.pop_back(); // restore scope.
            valtb.detag(); vartb.detag();
            proctb.detag(), functb.detag();
        }
    }
}

void pl0_tac_function_stmt(pl0_ast_function_stmt const *stmts) {
    cout << __func__;
    for (auto && f: stmts->funcs) {
        // validate.
        string fid = f.first->id->id;
        if (valtb.find(fid, false) || vartb.find(fid, false) || proctb.find(fid, false) || functb.find(fid, false)
                || (!scope.empty() && fid == scope.back())) {
            pl0_ast_error(f.first->id->loc, string("overloaded identifier ") + "\"" + fid + "\"" + " isn't a function");
        }
        else {
            scope.emplace_back(fid); // update global scope.
            vector<string> functype = pl0_tac_function_header(f.first);
            irb.emit("function", scope_name());
            functb.push(func(scope_name(), f.first->type->type, functype));
            proctb.tag(); functb.tag();
            irb.emit("allocret", scope_name());
            pl0_tac_prog(f.second);
            irb.emit("loadret", scope_name());
            irb.emit("endfunc", scope_name());
            // end of current scope.
            scope.pop_back(); // restore scope.
            valtb.detag(); vartb.detag();
            proctb.detag(), functb.detag();
        }
    }
}

vector<string> pl0_tac_procedure_header(pl0_ast_procedure_header const *header) {
    cout << __func__;
    return pl0_tac_param(header->params);
}

vector<string> pl0_tac_function_header(pl0_ast_function_header const *header) {
    cout << __func__;
    return pl0_tac_param(header->params);
}

vector<string> pl0_tac_param(pl0_ast_param_list const *param) {
    cout << __func__;
    // set up new flag, new scope.
    valtb.tag(); vartb.tag();
    
    if (!param) { return vector<string>(); } // no parameter.
    vector<string> type;
    for (auto && group: param->params) {
        for (auto && id: group->ids) {
            // validte.
            if (vartb.find(id->id, false) || id->id == scope.back()) {
                pl0_ast_error(id->loc, string("duplicate parameter ") + id->id);
            }
            vartb.push(variable(id->id, group->type->type)); // add parameters to symbol table.
            if (group->is_ref) {
                type.emplace_back(string("ref_") + group->type->type);
                irb.emit("paramref", id->id, group->type->type);
            }
            else {
                type.emplace_back(group->type->type);
                irb.emit("param", id->id, group->type->type);
            }
        }
    }
    return type;
}

void pl0_tac_compound_stmt(pl0_ast_compound_stmt const *compound_stmt) {
    cout << __func__;
    for (auto && stmt: compound_stmt->stmt) {
        pl0_tac_stmt(stmt);
    }
}

void pl0_tac_stmt(pl0_ast_stmt const *stmt) {
    cout << __func__;
    if (stmt == nullptr) { return; } // check if null pointer.
    switch (stmt->t) {
        case pl0_ast_stmt::type_t::ASSIGN_STMT: pl0_tac_assign_stmt(dynamic_cast<pl0_ast_assign_stmt const *>(stmt)); break;
        case pl0_ast_stmt::type_t::COND_STMT: pl0_tac_cond_stmt(dynamic_cast<pl0_ast_cond_stmt const *>(stmt)); break;
        case pl0_ast_stmt::type_t::CASE_STMT: pl0_tac_case_stmt(dynamic_cast<pl0_ast_case_stmt const *>(stmt)); break;
        case pl0_ast_stmt::type_t::CALL_PROC: pl0_tac_call_proc(dynamic_cast<pl0_ast_call_proc const *>(stmt)); break;
        case pl0_ast_stmt::type_t::COMPOUND_STMT: pl0_tac_compound_stmt(dynamic_cast<pl0_ast_compound_stmt const *>(stmt)); break;
        case pl0_ast_stmt::type_t::WRITE_STMT: pl0_tac_write_stmt(dynamic_cast<pl0_ast_write_stmt const *>(stmt)); break;
        case pl0_ast_stmt::type_t::READ_STMT: pl0_tac_read_stmt(dynamic_cast<pl0_ast_read_stmt const *>(stmt)); break;
        case pl0_ast_stmt::type_t::FOR_STMT: pl0_tac_for_stmt(dynamic_cast<pl0_ast_for_stmt const *>(stmt)); break;
        case pl0_ast_stmt::type_t::NULL_STMT: pl0_tac_null_stmt(dynamic_cast<pl0_ast_null_stmt const *>(stmt)); break;
        default: cout << "UNIMPLEMENT STMT TYPE" << endl;
    }
}

void pl0_tac_assign_stmt(pl0_ast_assign_stmt const *stmt) {
    variable var;
    Value *val = pl0_tac_expr(stmt->val).first;
    if (stmt->idx == nullptr && functb.depth(stmt->id->id) > vartb.depth(stmt->id->id)) {
        // set function's retval
        if (functb.find(stmt->id->id, true) == false) {
            pl0_ast_error(stmt->id->loc, string("use of undeclared function ") + "\"" + stmt->id->id + "\"");
        }
        irb.emit("setret", val);
    }
    else {
        // just simple assign.
        if (vartb.find(stmt->id->id, true, var) == false) {
            pl0_ast_error(stmt->id->loc, string("use of undeclared identifier ") + "\"" + stmt->id->id + "\"");
        }
        // assign to array element.
        if (stmt->idx) {
            if (var.len == -1) {
                pl0_ast_error(stmt->id->loc, string("treat ordinary variable ") + "\"" + stmt->id->id + "\" as an array");
            }
            Value *idx = pl0_tac_expr(stmt->idx).first;
            irb.emit("[]=", new Value(stmt->id->id), idx, val);
        }
        // assign to variable.
        else {
            if (var.len != -1) {
                pl0_ast_error(stmt->id->loc, string("expected an non-array identifier ") + "\"" + stmt->id->id + "\"");
            }
            irb.emit("=", new Value(stmt->id->id), val);
        }
    }
}

void pl0_tac_cond_stmt(pl0_ast_cond_stmt const *stmt) {
    cout << __func__;
    pl0_tac_condition(stmt->cond);
    int thenlabel = irb.makelabel();
    int elselabel = irb.makelabel();
    int endlabel = irb.makelabel();
    // reference: http://unixwiz.net/techtips/x86-jumps.html
    // +--------+------------------------------+-------------+--------------------+
    // |Instr   | Description                  | signed-ness | Flags              |
    // +--------+------------------------------+-------------+--------------------+
    // | JL/    | Jump if less                 | signed      | SF <> OF           |
    // | JNGE   | Jump if not greater or equal |             |                    |
    // +--------+------------------------------+-------------+--------------------+
    // | JGE/   | Jump if greater or equal     | signed      | SF = OF            |
    // | JNL    | Jump if not less             |             |                    |
    // +--------+------------------------------+-------------+--------------------+
    // | JLE/   | Jump if less or equal        | signed      | ZF = 1 or SF <> OF |
    // | JNG    | Jump if not greater          |             |                    |
    // +--------+------------------------------+-------------+--------------------+
    // | JG/    | Jump if greater              | signed      | ZF = 0 and SF = OF |
    // | JNLE   | Jump if not less or equal    |             |                    |
    // +--------+------------------------------+-------------+--------------------+
    if (stmt->cond->op->op == "<") {
        irb.emit("goto", new Value("jge"), new Value(elselabel));
    }
    else if (stmt->cond->op->op == "<=") {
        irb.emit("goto", new Value("jg"), new Value(elselabel));
    }
    else if (stmt->cond->op->op == ">") {
        irb.emit("goto", new Value("jle"), new Value(elselabel));
    }
    else if (stmt->cond->op->op == ">=") {
        irb.emit("goto", new Value("jl"), new Value(elselabel));
    }
    else if (stmt->cond->op->op == "=") {
        irb.emit("goto", new Value("jne"), new Value(elselabel));
    }
    else if (stmt->cond->op->op == "<>") {
        irb.emit("goto", new Value("je"), new Value(elselabel));
    }
    irb.emitlabel(thenlabel);
    pl0_tac_stmt(stmt->then_block);
    irb.emit("goto", new Value("jmp"), new Value(endlabel));
    irb.emitlabel(elselabel);
    pl0_tac_stmt(stmt->else_block);
    irb.emit("goto", new Value("jmp"), new Value(endlabel));
    irb.emitlabel(endlabel);
}

void pl0_tac_case_stmt(pl0_ast_case_stmt const *stmt) {
    cout << __func__;
    Value *val = pl0_tac_expr(stmt->expr).first;
    int t, endlabel = irb.makelabel();
    irb.emit("switch", val, new Value(irb.maketmp()), new Value(endlabel));
    std::vector<int> labels;
    for (auto item: stmt->terms) {
        t = irb.makelabel();
        irb.emit("case", new Value(item->constv->val), new Value(t));
        labels.emplace_back(t);
    }
    for (size_t i = 0; i < labels.size(); ++i) {
        irb.emitlabel(labels[i]);
        pl0_tac_stmt(stmt->terms[i]->stmt);
        irb.emit("goto", new Value("jmp"), new Value(endlabel));
    }
    irb.emit("endswitch", new Value(endlabel));
}

void pl0_tac_call_proc(pl0_ast_call_proc const *stmt) {
    cout << __func__;
    std::vector<std::pair<Value *, std::string>> args;
    if (stmt->args) {
        for (auto argexpr: stmt->args->args) {
            args.emplace(args.begin(), pl0_tac_expr(argexpr->arg));
        }
    }
    proc p;
    proctb.find(stmt->id->id, true, p);
    if (args.size() != p.param_t.size()) {
        pl0_ast_error(stmt->args->loc, string("unmatched number of parameters and arguments."));
    }
    else {
        for (size_t i = 0; i < args.size(); ++i) {
            bool is_ref = p.param_t[i].length() > 4 && p.param_t[i].substr(0, 4) == "ref_";
            if (args[i].second != (is_ref ? p.param_t[i].substr(4): p.param_t[i])) {
                pl0_ast_error(stmt->args->args[i]->loc, string("unmatched type of parameter and argument."));
            }
            if (is_ref) {
                if (args[i].first->t == Value::TYPE::INT || args[i].first->sv[0] == '~') {
                    pl0_ast_error(stmt->args->args[i]->loc, string("use constant or expression as reference value."));
                }
                irb.emit("pushref", args[i].first);
            }
            else {
                irb.emit("push", args[i].first);
            }
        }
    }
    irb.emit("callproc", p.name);
    for (auto arg: args) {
        irb.emit("pop", arg.first);
    }
}

void pl0_tac_for_stmt(pl0_ast_for_stmt const *stmt) {
    cout << __func__;
    // code structure: compare -> execute -> compare again
    int beginlabel = irb.makelabel();
    int endlabel = irb.makelabel();
    // validate loop iterator.
    if (vartb.find(stmt->iter->id, true) == false) {
        pl0_ast_error(stmt->iter->loc, string("undeclared identifier ") + "\"" + stmt->iter->id + "\"");
    }
    pair<Value *, string> s = pl0_tac_expr(stmt->initial);
    if (s.second != "integer" && s.second != "char") {
        pl0_ast_error(stmt->initial->loc, "use array as initial value in for loop");
    }
    irb.emit("forbegin", new Value(stmt->iter->id), s.first);
    irb.emitlabel(beginlabel);
    pair<Value *, string> t = pl0_tac_expr(stmt->end);
    if (t.second != "integer" && t.second != "char") {
        pl0_ast_error(stmt->end->loc, "use array as end value in for loop");
    }
    irb.emit("cmp", new Value(irb.maketmp()), new Value(stmt->iter->id), t.first);
    if (stmt->step->val == 1) {
        irb.emit("goto", new Value("jg"), new Value(endlabel));
    }
    else {
        irb.emit("goto", new Value("jl"), new Value(endlabel));
    }
    pl0_tac_stmt(stmt->stmt);
    irb.emit("+", new Value(stmt->iter->id), new Value(stmt->iter->id), new Value(stmt->step->val));
    irb.emit("forend", new Value("jmp"), new Value(beginlabel));
    irb.emitlabel(endlabel);
}

void pl0_tac_read_stmt(pl0_ast_read_stmt const *stmt) {
    cout << __func__;
    for (auto && id: stmt->ids) {
        // validate.
        variable var;
        if (vartb.find(id->id, true, var) == false) {
            pl0_ast_error(id->loc, string("use of undeclared variable ") + "\"" + id->id + "\"");
        }
        if (var.len != -1) {
            pl0_ast_error(id->loc, string("expected an non-array identifier ") + "\"" + id->id + "\"");
        }
        irb.emit("read", id->id);
    }
}

void pl0_tac_write_stmt(pl0_ast_write_stmt const *stmt) {
    cout << __func__;
    switch (stmt->t) {
        case pl0_ast_write_stmt::type_t::ONLY_STRING:
            irb.emit("write_s", new Value(stmt->str->val), new Value(irb.makelabel()));
            break;
        case pl0_ast_write_stmt::type_t::ONLY_EXPR:
            irb.emit("write_e", pl0_tac_expr(stmt->expr).first);
            break;
        case pl0_ast_write_stmt::type_t::STRING_AND_EXPR:
            irb.emit("write_s", new Value(stmt->str->val), new Value(irb.makelabel()));
            irb.emit("write_e", pl0_tac_expr(stmt->expr).first);
            break;
        default: cout << "UNIMPLEMENT WRITE TYPE" << endl;
    }
}

void pl0_tac_null_stmt(pl0_ast_null_stmt const *) {
    cout << __func__;
}

pair<Value *, string> pl0_tac_expr(pl0_ast_expression const *expr) {
    cout << __func__;
    bool needtmp = true;
    std::string t = "";
    pair<Value *, string> head = pl0_tac_term(expr->terms[0].second);
    Value *ans = head.first, *prev = head.first;
    if (expr->terms[0].first->op == '-') {
        ans = new Value(irb.maketmp());
        irb.emit("-", ans, new Value(0), prev);
        needtmp = false; // no more temporary variable.
    }
    if (expr->terms.size() > 1) {
        for (size_t i = 1; i < expr->terms.size(); ++i) {
            if (needtmp) { ans = new Value(irb.maketmp()); }
            auto element = pl0_tac_term(expr->terms[i].second);
            if (element.second != head.second) {
                pl0_ast_error(expr->terms[i].second->loc, "do +/- operation on two terms with different types.");
            }
            irb.emit(string(1, expr->terms[i].first->op), ans, needtmp ? prev : ans, element.first);
            needtmp = false; // no more temporary variable.
        }
    }
    return make_pair(ans, head.second);
}

void pl0_tac_condition(pl0_ast_condtion const *cond) {
    cout << __func__;
    auto lhs = pl0_tac_expr(cond->lhs);
    auto rhs = pl0_tac_expr(cond->rhs);
    cout << ";; compare type: " << lhs.second <<  "  "  << rhs.second << endl;
    if (lhs.second != rhs.second) {
        pl0_ast_error(cond->loc, "compare two expressions with different types.");
    }
    irb.emit("cmp", new Value(irb.maketmp()), lhs.first, rhs.first);
}

pair<Value *, string> pl0_tac_term(pl0_ast_term const *term) {
    cout << __func__;
    bool needtmp = true;
    pair<Value *, string> head = pl0_tac_factor(term->factors[0].second);
    Value *ans = head.first, *prev = head.first;
    if (term->factors.size() > 1) {
        for (size_t i = 1; i < term->factors.size(); ++i) {
            if (needtmp) { ans = new Value(irb.maketmp()); }
            auto element = pl0_tac_factor(term->factors[i].second);
            if (element.second != head.second) {
                pl0_ast_error(term->factors[i].second->loc, "do *// operation on two factors with different types.");
            }
            irb.emit(string(1, term->factors[i].first->op), ans, needtmp ? prev : ans, element.first);
            needtmp = false; // no more temporary variable.
        }
    }
    return make_pair(ans, head.second);
}

pair<Value *, string> pl0_tac_factor(pl0_ast_factor const * factor) {
    cout << __func__;
    pair<Value *, string> ans;
    // variables used inner switch-case block.
    int d1, d2;
    constant val;
    variable var, array;
    Value *idx, *t;

    switch (factor->t) {
        case pl0_ast_factor::type_t::ID:
            d1 = valtb.depth(factor->ptr.id->id), d2 = vartb.depth(factor->ptr.id->id);
            if (d1 > d2) {
                // constant
                valtb.find(factor->ptr.id->id, true, val);
                ans = make_pair(new Value(val.val), "integer");
            }
            else if (d2 > d1) {
                // variable
                vartb.find(factor->ptr.id->id, true, var);
                if (var.type.length() >= 5 && var.type.substr(0, 5) == "array") {
                    pl0_ast_error(factor->loc, string("use an array identifier ") + "\"" + var.name + "\"" + " as a factor.");
                }
                ans = make_pair(new Value(var.name), var.type);
            }
            else {
                // undeclared identifier
                pl0_ast_error(factor->loc, string("use of undeclared identifier ") + "\"" + factor->ptr.id->id + "\"");
                ans = make_pair(new Value(factor->ptr.id->id), "");
            }
            break;
        case pl0_ast_factor::type_t::UNSIGNED:
            ans = make_pair(new Value(factor->ptr.unsignedn->val), "integer");
            break;
        case pl0_ast_factor::type_t::EXPR:
            ans = pl0_tac_expr(factor->ptr.expr);
            break;
        case pl0_ast_factor::type_t::CALL_FUNC:
            ans = pl0_tac_call_func(factor->ptr.call_func);
            break;
        case pl0_ast_factor::type_t::ARRAY_E:
            // validate array identifier.
            if (vartb.find(factor->arraye.first->id, true, array)) {
                if (array.len == -1) {
                    pl0_ast_error(factor->loc, "expected an array identifier.");
                }
            }
            else {
                pl0_ast_error(factor->loc, string("use of undeclared identifier ") + "\"" + factor->arraye.first->id + "\"");
                array.name = factor->arraye.first->id; // give it a default value, although it's wrong.
            }
            // validate array index.
            idx = pl0_tac_expr(factor->arraye.second).first;                
            t = new Value(irb.maketmp());
            irb.emit("=[]", t, new Value(array.name), idx);
            ans = make_pair(t, array.type.substr(0, array.type.length()-5));
            break;
        default: ans = make_pair(new Value("^^^^^"), ""); cout << "UNIMPLEMENT EXPRESSION" << endl;
    }
    return ans;
}

pair<Value *, string> pl0_tac_call_func(pl0_ast_call_func const *stmt) {
    cout << __func__;
    // a single identifier can be a function id or just simple variable. STRATEGY: choose the nested one.
    string fid = stmt->fn->id;
    if (!stmt->args || stmt->args->args.empty()) {
        int d1 = valtb.depth(fid), d2 = vartb.depth(fid), d3 = functb.depth(fid);
        if (d1 > d2 && d1 > d3) {
            constant val; valtb.find(fid, true, val);
            return make_pair(new Value(val.val), "integer");
        }
        if (d2 > d3) {
            variable var; vartb.find(fid, true, var);
            if (var.len != -1) {
                pl0_ast_error(stmt->loc, string("use an array ") + "\"" + var.name + "\"" + " as a factor.");
            }
            return make_pair(new Value(var.name), var.type);
        }
    }

    if (!functb.find(fid, true)) {
        pl0_ast_error(stmt->loc, string("use of undeclared identifier ") + "\"" + fid + "\"");
    }
    std::vector<std::pair<Value *, std::string>> args;
    if (stmt->args) {
        for (auto argexpr: stmt->args->args) {
            auto arg = pl0_tac_expr(argexpr->arg);

            args.emplace(args.begin(), arg);
        }
    }
    func f;
    functb.find(fid, true, f);
    if (args.size() != f.param_t.size()) {
        pl0_ast_error(stmt->args->loc, string("unmatched number of parameters and arguments."));
    }
    else {
        for (size_t i = 0; i < args.size(); ++i) {
            bool is_ref = f.param_t[i].length() > 4 && f.param_t[i].substr(0, 4) == "ref_";
            if (args[i].second != (is_ref ? f.param_t[i].substr(4) : f.param_t[i])) {
                pl0_ast_error(stmt->args->args[i]->loc, string("unmatched type of parameter and argument."));
            }
            if (is_ref) {
                if (args[i].first->t == Value::TYPE::INT || args[i].first->sv[0] == '~') {
                    pl0_ast_error(stmt->args->args[i]->loc, string("use constant or expression as reference value."));
                }
                irb.emit("pushref", args[i].first);
            }
            else {
                irb.emit("push", args[i].first);
            }
        }
    }
    string retval = irb.makeret();
    irb.emit("callfunc", f.name, retval);
    for (auto arg: args) {
        irb.emit("pop", arg.first);
    }
    func fn;
    functb.find(fid, true, fn);
    return make_pair(new Value(retval), fn.rettype);
}

#endif /* __PLO_TAC_GEN_HPP__ */


