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
pair<Value *, string> pl0_tac_term(pl0_ast_term const *term);
pair<Value *, string> pl0_tac_factor(pl0_ast_factor const * factor);
pair<Value *, string> pl0_tac_call_func(pl0_ast_call_func const *stmt);

extern struct IRBuilder irb;

static bool status = true;

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
    irb.emit("exit", new Value(0, "integer")); // main function: exit with 0.
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
    irb.emitlabel(irb.makelabel());
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
            valtb.push(constant(id, def->val->val, (def->val->dt == pl0_ast_constv::INT ? "integer" : "char"))); // update symbol table.
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
                vartb.push(variable(var->id, s->type->type->type, t, s->type->len)); // update symbol table.
                irb.emit("def", new Value(var->id, s->type->type->type), new Value(t, "string"), new Value(s->type->len, "integer"));
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
            irb.emit("procedure", new Value(scope_name(), "string"));
            proctb.push(proc(scope_name(), proctype)); // update symbol table.
            proctb.tag(); functb.tag();
            pl0_tac_prog(p.second);
            irb.emit("endproc", new Value(scope_name(), "string"));
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
            functb.push(func(scope_name(), f.first->type->type, functype)); // update symbol table
            proctb.tag(); functb.tag();
            irb.emit("def", new Value(scope_name(), f.first->type->type), new Value("function", "string"), new Value(-1, "integer"));
            pl0_tac_prog(f.second);
            irb.emit("loadret", new Value(scope_name(), "string"));
            irb.emit("endfunc", new Value(scope_name(), "string"));
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
            vartb.push(variable(id->id, group->type->type, group->type->type)); // add parameters to symbol table.
            if (group->is_ref) {
                type.emplace_back(string("ref_") + group->type->type);
                irb.emit("paramref", new Value(id->id, group->type->type), new Value(group->type->type, "string"));
            }
            else {
                type.emplace_back(group->type->type);
                irb.emit("param", new Value(id->id, group->type->type), new Value(group->type->type, "string"));
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
        func f;
        if (functb.find(stmt->id->id, true, f) == false) {
            pl0_ast_error(stmt->id->loc, string("use of undeclared function ") + "\"" + stmt->id->id + "\"");
        }
        irb.emit("=", new Value(f.name, f.rettype), val);
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
            irb.emit("[]=", new Value(stmt->id->id, var.dt), idx, val);
        }
        // assign to variable.
        else {
            if (var.len != -1) {
                pl0_ast_error(stmt->id->loc, string("expected an non-array identifier ") + "\"" + stmt->id->id + "\"");
            }
            irb.emit("=", new Value(stmt->id->id, var.dt), val);
        }
    }
}

void pl0_tac_cond_stmt(pl0_ast_cond_stmt const *stmt) {
    cout << __func__;

    int thenlabel = irb.makelabel(), elselabel, endlabel = irb.makelabel();
    
    auto lhs = pl0_tac_expr(stmt->cond->lhs);
    auto rhs = pl0_tac_expr(stmt->cond->rhs);
    // if (lhs.second != rhs.second) {
    //     pl0_ast_error(stmt->cond->loc, "compare two expressions with different types.");
    // }
    irb.emit("cmp", new Value(thenlabel, "integer"), lhs.first, rhs.first);

    if (stmt->else_block == nullptr) {
        elselabel = endlabel;
    }
    else {
        elselabel = irb.makelabel();
    }
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
        irb.emit("goto", new Value("jge", "string"), new Value(elselabel, "integer"));
    }
    else if (stmt->cond->op->op == "<=") {
        irb.emit("goto", new Value("jg", "string"), new Value(elselabel, "integer"));
    }
    else if (stmt->cond->op->op == ">") {
        irb.emit("goto", new Value("jle", "string"), new Value(elselabel, "integer"));
    }
    else if (stmt->cond->op->op == ">=") {
        irb.emit("goto", new Value("jl", "string"), new Value(elselabel, "integer"));
    }
    else if (stmt->cond->op->op == "=") {
        irb.emit("goto", new Value("jne", "string"), new Value(elselabel, "integer"));
    }
    else if (stmt->cond->op->op == "<>") {
        irb.emit("goto", new Value("je", "string"), new Value(elselabel, "integer"));
    }
    irb.emitlabel(thenlabel);
    pl0_tac_stmt(stmt->then_block);
    irb.emit("goto", new Value("jmp", "string"), new Value(endlabel, "integer"));
    if (stmt->else_block != nullptr) {
        irb.emitlabel(elselabel);
        pl0_tac_stmt(stmt->else_block);
        irb.emit("goto", new Value("jmp", "string"), new Value(endlabel, "integer"));
    }
    irb.emitlabel(endlabel);
}

void pl0_tac_case_stmt(pl0_ast_case_stmt const *stmt) {
    cout << __func__;
    auto case_cond = pl0_tac_expr(stmt->expr);
    // add case condition value to symbol table.
    Value *cond;
    if (case_cond.first->t == Value::TYPE::STR && case_cond.first->sv[0] == '~') {
        cond = new Value(string("~") + case_cond.first->sv, case_cond.second);
        irb.emit("def", cond, new Value(case_cond.second, "string"), new Value(-1, "integer"));
        irb.emit("=", cond, case_cond.first);
    }
    else {
        cond = case_cond.first;
    }
    int endlabel = irb.makelabel();
    std::vector<int> labels;
    for (auto && item: stmt->terms) {
        labels.emplace_back(irb.makelabel());
    }
    labels.emplace_back(endlabel);
    irb.emit("goto", new Value("jmp", "string"), new Value(labels[0], "integer"));
    for (size_t i = 0; i < stmt->terms.size(); ++i) {
        int t = irb.makelabel();
        irb.emitlabel(labels[i]);
        irb.emit("cmp", new Value(t, "integer"), cond, new Value(stmt->terms[i]->constv->val, "integer"));
        irb.emit("goto", new Value("jne", "string"), new Value(labels[i+1], "integer"));
        irb.emitlabel(t);
        pl0_tac_stmt(stmt->terms[i]->stmt);
        irb.emit("goto", new Value("jmp", "string"), new Value(endlabel, "integer"));
    }
    irb.emitlabel(endlabel);
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
    std::vector<std::pair<Value *, bool>> pushes; // <Value * val, bool is_ref>
    if (args.size() != p.param_t.size()) {
        pl0_ast_error(stmt->args->loc, string("unmatched number of parameters and arguments."));
    }
    else {
        size_t len = args.size();
        for (size_t i = 0; i < len; ++i) {
            bool is_ref = p.param_t[len-1-i].length() > 4 && p.param_t[len-1-i].substr(0, 4) == "ref_";
            // if (args[i].second != (is_ref ? p.param_t[len-1-i].substr(4): p.param_t[len-1-i])) {
            //     pl0_ast_error(stmt->args->args[i]->loc, string("unmatched type of parameter and argument."));
            // }
            if (is_ref) {
                if (args[i].first->t == Value::TYPE::IMM || args[i].first->sv[0] == '~') {
                    pl0_ast_error(stmt->args->args[i]->loc, string("use constant or expression as reference value."));
                }
                pushes.emplace_back(make_pair(args[i].first, true));
            }
            else {
                pushes.emplace_back(make_pair(args[i].first, false));
            }
        }
    }
    irb.emit("call", new Value(p.name, "string"), pushes);
    irb.emit("label", new Value(irb.makelabel(), "integer"), new Value("allsuffix", "string"));
}

void pl0_tac_for_stmt(pl0_ast_for_stmt const *stmt) {
    cout << __func__;
    // code structure: compare -> execute -> compare again
    int beginlabel = irb.makelabel();
    int innerlabel = irb.makelabel();
    int endlabel = irb.makelabel();
    // validate loop iterator.
    variable var;
    if (vartb.find(stmt->iter->id, true, var) == false) {
        pl0_ast_error(stmt->iter->loc, string("undeclared identifier ") + "\"" + stmt->iter->id + "\"");
    }
    pair<Value *, string> s = pl0_tac_expr(stmt->initial);
    if (s.second != "integer" && s.second != "char") {
        pl0_ast_error(stmt->initial->loc, "use array as initial value in for loop");
    }
    irb.emit("=", new Value(stmt->iter->id, var.dt), s.first);
    irb.emit("goto", new Value("jmp", "string"), new Value(beginlabel, "integer"));
    irb.emitlabel(beginlabel);
    pair<Value *, string> t = pl0_tac_expr(stmt->end);
    if (t.second != "integer" && t.second != "char") {
        pl0_ast_error(stmt->end->loc, "use array as end value in for loop");
    }
    // add end value to symbol table.
    Value *end;
    if (t.first->t == Value::TYPE::STR && t.first->sv[0] == '~') {
        end = new Value("~" + t.first->sv, t.second);
        irb.emit("def", end, new Value(t.second, "string"), new Value(-1, "integer"));
        irb.emit("=", end, t.first);
    }
    else {
        end = t.first;
    }
    irb.emit("cmp", new Value(innerlabel, "integer"), new Value(stmt->iter->id, var.dt), end);
    if (stmt->step->val == 1) {
        irb.emit("goto", new Value("jg", "string"), new Value(endlabel, "integer"));
    }
    else {
        irb.emit("goto", new Value("jl", "string"), new Value(endlabel, "integer"));
    }
    irb.emitlabel(innerlabel); // label for inner executable block.
    pl0_tac_stmt(stmt->stmt);
    irb.emit("+", new Value(stmt->iter->id, var.dt), new Value(stmt->iter->id, var.dt), new Value(stmt->step->val, "integer"));
    irb.emit("goto", new Value("jmp", "string"), new Value(beginlabel, "integer"));
    irb.emitlabel(endlabel);
    irb.emit("=", new Value(stmt->iter->id, var.dt), t.first);
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
            irb.emit("write_s", new Value(stmt->str->val, "string"), new Value(irb.makelabel(), "integer"));
            break;
        case pl0_ast_write_stmt::type_t::ONLY_EXPR:
            irb.emit("write_e", pl0_tac_expr(stmt->expr).first);
            break;
        case pl0_ast_write_stmt::type_t::STRING_AND_EXPR:
            irb.emit("write_s", new Value(stmt->str->val, "string"), new Value(irb.makelabel(), "integer"));
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
        ans = new Value(irb.maketmp(), head.second);
        irb.emit("-", ans, new Value(0, head.second), prev);
        needtmp = false; // no more temporary variable.
    }
    if (expr->terms.size() > 1) {
        for (size_t i = 1; i < expr->terms.size(); ++i) {
            if (needtmp) { ans = new Value(irb.maketmp(), head.second); }
            auto element = pl0_tac_term(expr->terms[i].second);
            // if (element.second != head.second) {
            //     pl0_ast_error(expr->terms[i].second->loc, "do +/- operation on two terms with different types.");
            // }
            irb.emit(string(1, expr->terms[i].first->op), ans, needtmp ? prev : ans, element.first);
            needtmp = false; // no more temporary variable.
        }
    }
    return make_pair(ans, head.second);
}

pair<Value *, string> pl0_tac_term(pl0_ast_term const *term) {
    cout << __func__;
    bool needtmp = true;
    pair<Value *, string> head = pl0_tac_factor(term->factors[0].second);
    Value *ans = head.first, *prev = head.first;
    if (term->factors.size() > 1) {
        for (size_t i = 1; i < term->factors.size(); ++i) {
            if (needtmp) { ans = new Value(irb.maketmp(), head.second); }
            auto element = pl0_tac_factor(term->factors[i].second);
            // if (element.second != head.second) {
            //     pl0_ast_error(term->factors[i].second->loc, "do *// operation on two factors with different types.");
            // }
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
                ans = make_pair(new Value(val.val, val.dt), "integer");
            }
            else if (d2 > d1) {
                // variable
                vartb.find(factor->ptr.id->id, true, var);
                if (var.type.length() >= 5 && var.type.substr(0, 5) == "array") {
                    pl0_ast_error(factor->loc, string("use an array identifier ") + "\"" + var.name + "\"" + " as a factor.");
                }
                ans = make_pair(new Value(var.name, var.dt), var.dt);
            }
            else {
                // undeclared identifier
                pl0_ast_error(factor->loc, string("use of undeclared identifier ") + "\"" + factor->ptr.id->id + "\"");
                ans = make_pair(new Value(factor->ptr.id->id, "undefined"), "");
            }
            break;
        case pl0_ast_factor::type_t::UNSIGNED:
            ans = make_pair(new Value(factor->ptr.unsignedn->val, factor->ptr.unsignedn->dt == pl0_ast_constv::INT ? "integer" : "char"), "integer");
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
            t = new Value(irb.maketmp(), array.dt);
            irb.emit("=[]", t, new Value(array.name, array.dt), idx);
            ans = make_pair(t, array.type.substr(0, array.type.length()-5));
            break;
        default:
            pl0_ast_error(factor->loc, "undefined syntax");
            ans = make_pair(new Value("^^^^^", "undefined"), "");
            cout << "UNIMPLEMENT EXPRESSION" << endl;
            break;
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
            return make_pair(new Value(val.val, val.dt), "integer");
        }
        if (d2 > d3) {
            variable var; vartb.find(fid, true, var);
            if (var.len != -1) {
                pl0_ast_error(stmt->loc, string("use an array ") + "\"" + var.name + "\"" + " as a factor.");
            }
            return make_pair(new Value(var.name, var.dt), var.type);
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
    func fn;
    functb.find(fid, true, fn);
    std::vector<std::pair<Value *, bool>> pushes; // <Value * val, bool is_ref>
    if (args.size() != fn.param_t.size()) {
        pl0_ast_error(stmt->args->loc, string("unmatched number of parameters and arguments."));
    }
    else {
        size_t len = args.size();
        for (size_t i = 0; i < args.size(); ++i) {
            bool is_ref = fn.param_t[len-1-i].length() > 4 && fn.param_t[len-1-i].substr(0, 4) == "ref_";
            // if (args[i].second != (is_ref ? fn.param_t[len-1-i].substr(4) : fn.param_t[len-1-i])) {
            //     pl0_ast_error(stmt->args->args[i]->loc, string("unmatched type of parameter and argument."));
            // }
            if (is_ref) {
                if (args[i].first->t == Value::TYPE::IMM || args[i].first->sv[0] == '~') {
                    pl0_ast_error(stmt->args->args[i]->loc, string("use constant or expression as reference value."));
                }
                pushes.emplace_back(make_pair(args[i].first, true));
            }
            else {
                pushes.emplace_back(make_pair(args[i].first, false));
            }
        }
    }
    string retval = irb.makeret();
    irb.emit("call", new Value(fn.name, "string"), pushes, new Value(retval, fn.rettype));
    irb.emit("label", new Value(irb.makelabel(), "integer"), new Value("allsuffix", "string"));
    return make_pair(new Value(retval, fn.rettype), fn.rettype);
}

#endif /* __PLO_TAC_GEN_HPP__ */


