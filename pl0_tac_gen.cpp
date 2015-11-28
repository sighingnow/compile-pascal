/**
 * TAC (Three Address Code, the intermedia reperesentation format) generators.
 */

#ifndef __PL0_TAC_GEN_HPP__
#define __PL0_TAC_GEN_HPP__

#include <vector>
#include <string>
#include "pl0_ast.hpp"

#define __func__ __func__ << endl
// #define __func__ ""

bool pl0_tac_program(pl0_ast_program const *program);
void pl0_tac_prog(pl0_ast_prog const *prog);
void pl0_tac_const_stmt(pl0_ast_const_stmt const *stmts);
void pl0_tac_var_stmt(pl0_ast_var_stmt const *stmts);
void pl0_tac_procedure_stmt(pl0_ast_procedure_stmt const *stmts);
void pl0_tac_function_stmt(pl0_ast_function_stmt const *stmts);
string pl0_tac_procedure_header(pl0_ast_procedure_header const *header);
string pl0_tac_function_header(pl0_ast_function_header const *header);
string pl0_tac_param(pl0_ast_param_list const *param);
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
pair<string, string> pl0_tac_expr(pl0_ast_expression const *expr);
string pl0_tac_condition(pl0_ast_condtion const *cond);
pair<string, string> pl0_tac_term(pl0_ast_term const *term);
pair<string, string> pl0_tac_factor(pl0_ast_factor const * factor);
pair<string, string> pl0_tac_call_func(pl0_ast_call_func const *stmt);

extern struct IRBuilder irb;

static bool status = true;
static string ERROR[] = {

};

static std::string old = "", scope = "";
static pl0_env<struct variable> vartb;
static pl0_env<struct value> valtb;
static pl0_env<struct proc> proctb;
static pl0_env<struct func> functb;


void pl0_ast_error(pair<int, int> loc, string msg) {
    status = false;
    cout << loc.first << ":" << loc.second << " " << "ERRRO: " << msg << endl;
}

bool pl0_tac_program(pl0_ast_program const *program) {
    cout << __func__;
    irb.emit("begin program");
    pl0_tac_prog(program->program);
    irb.emit("end program");
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
        if (id == scope) {
            pl0_ast_error(def->id->loc, string("duplicate identifier ") + "\"" + id + "\"");
        }
        else if (valtb.find(id, false) || vartb.find(id, false)) {
            pl0_ast_error(def->id->loc, string("redefinition of ") + "\"" + id + "\"");
        }
        else {
            valtb.push(value(id, def->val->val)); // update symbol table.
            irb.emit("const " + id + ": " + std::to_string(def->val->val));
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
            if (var->id == scope) {
                pl0_ast_error(var->loc, string("duplicate identifier ") + "\"" + var->id + "\"");
            }
            else if (valtb.find(var->id, false) || vartb.find(var->id, false)) {
                pl0_ast_error(var->loc, string("redefinition of ") + "\"" + var->id + "\"");
            }
            else {
                string t = s->type->type->type + (s->type->len == -1 ? "" : "array " + std::to_string(s->type->len));
                vartb.push(variable(var->id, t, s->type->len)); // update symbol table.
                irb.emit("var " + var->id + ": " + t);
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
                || pid == scope) {
            pl0_ast_error(p.first->id->loc, string("overloaded identifier ") + "\"" + pid + "\"" + " isn't a function");
        }
        else {
            string proctype = pl0_tac_procedure_header(p.first);
            proctb.push(proc(pid, proctype)); // update symbol table.
            proctb.tag(); functb.tag();
            old = scope; scope = pid; // update global scope.
            pl0_tac_prog(p.second);
            irb.emit("endproc " + pid);
            // end of current scope.
            scope = old; // restore scope.
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
                || fid == scope) {
            pl0_ast_error(f.first->id->loc, string("overloaded identifier ") + "\"" + fid + "\"" + " isn't a function");
        }
        else {
            string functype = pl0_tac_function_header(f.first);
            functb.push(func(fid, f.first->type->type, functype));
            proctb.tag(); functb.tag();
            old = scope; scope = fid; // update global scope.
            pl0_tac_prog(f.second);
            irb.emit("endfunc " + fid);
            // end of current scope.
            scope = old; // restore scope.
            valtb.detag(); vartb.detag();
            proctb.detag(), functb.detag();
        }
    }
}

string pl0_tac_procedure_header(pl0_ast_procedure_header const *header) {
    cout << __func__;
    irb.emit("procedure " + header->id->id);
    return pl0_tac_param(header->params);
}

string pl0_tac_function_header(pl0_ast_function_header const *header) {
    cout << __func__;
    irb.emit("function " + header->id->id + ": " + header->type->type);
    return pl0_tac_param(header->params);
}

string pl0_tac_param(pl0_ast_param_list const *param) {
    cout << __func__;
    // set up new flag, new scope.
    valtb.tag(); vartb.tag();
    
    if (!param) { return ""; } // no parameter.
    string type = "";
    for (auto && group: param->params) {
        for (auto && id: group->ids) {
            vartb.push(variable(id->id, group->type->type)); // add parameters to symbol table.
            type = type + "_" + group->type->type;
            if (group->is_ref) {
                irb.emit("paramref " + group->type->type + " " + id->id);
            }
            else {
                irb.emit("param " + group->type->type + " " + id->id);
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
    string val = pl0_tac_expr(stmt->val).first;

    cout << "^^^^^ find: " << stmt->id->id << "   " << functb.depth(stmt->id->id) << "   " << vartb.depth(stmt->id->id) << endl;
    if (stmt->idx == nullptr && functb.depth(stmt->id->id) < vartb.depth(stmt->id->id)) {
        // set function's retval
        if (functb.find(stmt->id->id, true) == false) {
            pl0_ast_error(stmt->id->loc, string("use of undeclared function ") + "\"" + stmt->id->id + "\"");
        }
        irb.emit("setret " + val);
    }
    else {
        // just simple assign.
        if (vartb.find(stmt->id->id, true, var) == false) {
            pl0_ast_error(stmt->id->loc, string("use of undeclared identifier ") + "\"" + stmt->id->id + "\"");
        }
        if (var.len != -1) {
            pl0_ast_error(stmt->id->loc, string("expected an non-array identifier ") + "\"" + stmt->id->id + "\"");
        }
        // assign to variable.
        if (stmt->idx) {
            string idx = pl0_tac_expr(stmt->idx).first;
            irb.emit(stmt->id->id + " [] " + idx + " = " + val);
        }
        // assign to array element.
        else {
            irb.emit(stmt->id->id + " = " + val);
        }
    }
}

void pl0_tac_cond_stmt(pl0_ast_cond_stmt const *stmt) {
    cout << __func__;
    string cond = pl0_tac_condition(stmt->cond);
    int thenlabel = irb.makelabel();
    int elselabel = irb.makelabel();
    int endlabel = irb.makelabel();
    irb.emit("iffalse " + cond + " goto L" + std::to_string(elselabel));
    irb.emitlabel(thenlabel);
    pl0_tac_stmt(stmt->then_block);
    irb.emit("goto L" + std::to_string(endlabel));
    irb.emitlabel(elselabel);
    pl0_tac_stmt(stmt->else_block);
    irb.emit("goto L" + std::to_string(endlabel));
    irb.emitlabel(endlabel);
}

void pl0_tac_case_stmt(pl0_ast_case_stmt const *stmt) {
    cout << __func__;
    string val = pl0_tac_expr(stmt->expr).first;
    int endlabel = irb.makelabel();
    irb.emit("switch " + val + " case " + std::to_string(endlabel));
    for (auto item: stmt->terms) {
        irb.emit("case " + std::to_string(item->constv->val));
        irb.emitlabel(irb.makelabel());
        pl0_tac_stmt(item->stmt);
    }
    irb.emit("endcase");
}

void pl0_tac_call_proc(pl0_ast_call_proc const *stmt) {
    cout << __func__;
    std::vector<std::string> args;
    if (stmt->args) {
        for (auto argexpr: stmt->args->args) {
            args.emplace_back(pl0_tac_expr(argexpr->arg).first);
        }
    }
    for (auto arg: args) {
        irb.emit("push " + arg);
    }
    irb.emit("callproc " + stmt->id->id);
    for (auto arg: args) {
        irb.emit("pop " + arg);
    }
}

void pl0_tac_for_stmt(pl0_ast_for_stmt const *stmt) {
    cout << "TODO" << __func__;
    int beginlabel = irb.makelabel();
    int endlabel = irb.makelabel();
    irb.emitlabel(beginlabel);
    // TODO: test loop condition.
    pl0_tac_stmt(stmt->stmt);
    irb.emit("goto L" + std::to_string(beginlabel));
    irb.emitlabel(endlabel);
}

void pl0_tac_read_stmt(pl0_ast_read_stmt const *stmt) {
    cout << __func__;
    for (auto && id: stmt->ids) {
        // validate.
        variable var;
        if (vartb.find(id->id, true, var) == false) {
            pl0_ast_error(id->loc, string("use of undeclared identifier ") + "\"" + id->id + "\"");
        }
        if (var.len != -1) {
            pl0_ast_error(id->loc, string("expected an non-array identifier ") + "\"" + id->id + "\"");
        }
        irb.emit("read " + id->id);
    }
}

void pl0_tac_write_stmt(pl0_ast_write_stmt const *stmt) {
    cout << __func__;
    switch (stmt->t) {
        case pl0_ast_write_stmt::type_t::ONLY_STRING:
            irb.emit("writes " + stmt->str->val);
            break;
        case pl0_ast_write_stmt::type_t::ONLY_EXPR:
            irb.emit("writee " + pl0_tac_expr(stmt->expr).first);
            break;
        case pl0_ast_write_stmt::type_t::STRING_AND_EXPR:
            irb.emit("writes " + stmt->str->val);
            irb.emit("writee " + pl0_tac_expr(stmt->expr).first);
            break;
        default: cout << "UNIMPLEMENT WRITE TYPE" << endl;
    }
}

void pl0_tac_null_stmt(pl0_ast_null_stmt const *) {
    cout << __func__;
    irb.emit("null stmt");
}

pair<string, string> pl0_tac_expr(pl0_ast_expression const *expr) {
    cout << __func__;
    std::string t = "", prev = "", ans;
    for (auto term: expr->terms) {
        ans = irb.maketmp();
        auto element = pl0_tac_term(term.second);
        if (t == "") {
            t = element.second; // initial data type.
        }
        else if (t != element.second) {
            pl0_ast_error(term.second->loc, "do +/- operation on two terms with different types.");
        }
        irb.emit(ans + " = " + prev + string(1, term.first->op) + element.first);
        prev = ans;
    }
    return make_pair(ans, t);
}

string pl0_tac_condition(pl0_ast_condtion const *cond) {
    cout << __func__;
    auto lhs = pl0_tac_expr(cond->lhs);
    auto rhs = pl0_tac_expr(cond->rhs);
    if (lhs.second != rhs.second) {
        pl0_ast_error(cond->loc, "compare two expressions with different types.");
    }
    string t = irb.maketmp();
    irb.emit(t + " = " + lhs.first + cond->op->op + rhs.first);
    return t;
}

pair<string, string> pl0_tac_term(pl0_ast_term const *term) {
    cout << __func__;
    pair<string, string> head = pl0_tac_factor(term->factors[0].second);
    std::string ans = head.first, prev = head.first;
    if (term->factors.size() > (size_t)1) {
        for (size_t i = 1; i < term->factors.size(); ++i) {
            ans = irb.maketmp();
            auto element = pl0_tac_factor(term->factors[i].second);
            if (element.second != head.second) {
                pl0_ast_error(term->factors[i].second->loc, "do *// operation on two factors with different types.");
            }
            irb.emit(ans + " = " + prev + string(1, term->factors[i].first->op) + element.first);
            prev= ans;
        }
    }
    return make_pair(ans, head.second);
}

pair<string, string> pl0_tac_factor(pl0_ast_factor const * factor) {
    cout << __func__;
    pair<string, string> ans;
    // variables used inner switch-case block.
    variable var, array;
    std::string t;
    pair<string, string> idx;
    switch (factor->t) {
        case pl0_ast_factor::type_t::ID:
            if (vartb.find(factor->ptr.id->id, true)) {
                if (var.type.length() >= 5 && var.type.substr(0, 5) == "array") {
                    pl0_ast_error(factor->loc, string("use an array identifier ") + "\"" + var.name + "\"" + " as a factor.");
                }
                ans = make_pair(var.name, var.type);
            }
            else {
                pl0_ast_error(factor->loc, string("use of undeclared identifier ") + "\"" + factor->ptr.id->id + "\"");
                ans = make_pair(factor->ptr.id->id, "");
            }
            break;
        case pl0_ast_factor::type_t::UNSIGNED:
            ans = make_pair(std::to_string(factor->ptr.unsignedn->val), "integer");
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
            idx = pl0_tac_expr(factor->arraye.second);                
            t = irb.maketmp();
            irb.emit(t + " = " + array.name + " [] " + idx.first);
            ans = make_pair(t, array.type);
            break;
        default: ans = make_pair("^^^^", ""); cout << "UNIMPLEMENT EXPRESSION" << endl;
    }
    return ans;
}

pair<string, string> pl0_tac_call_func(pl0_ast_call_func const *stmt) {
    cout << __func__;
    // a single identifier can be a function id or just simple variable. STRATEGY: choose the nested one.
    string fid = stmt->fn->id;
    if (!stmt->args || stmt->args->args.empty()) {
        if (vartb.find(fid, true) && (!functb.find(fid, true) || vartb.depth(fid) < functb.depth(fid))) {
            variable var;
            vartb.find(fid, true, var);
            if (var.len != -1) {
                pl0_ast_error(stmt->loc, string("use an array ") + "\"" + var.name + "\"" + " as a factor.");
            }
            return make_pair(var.name, var.type);
        }
    }

    if (!functb.find(fid, true)) {
        pl0_ast_error(stmt->loc, string("use of undeclared identifier ") + "\"" + fid + "\"");
    }
    std::vector<std::string> args;
    if (stmt->args) {
        for (auto argexpr: stmt->args->args) {
            args.emplace_back(pl0_tac_expr(argexpr->arg).first);
        }
    }
    for (auto arg: args) {
        irb.emit("push " + arg);
    }
    string retval = irb.makeret();
    irb.emit("allocret " + retval);
    irb.emit("callfunc " + fid);
    for (auto arg: args) {
        irb.emit("pop " + arg);
    }
    func fn;
    functb.find(fid, true, fn);
    return make_pair(retval, fn.rettype);
}

#endif /* __PLO_TAC_GEN_HPP__ */


