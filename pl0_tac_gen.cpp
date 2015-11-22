/**
 * TAC (Three Address Code, the intermedia reperesentation format) generators.
 */

#ifndef __PL0_TAC_GEN_HPP__
#define __PL0_TAC_GEN_HPP__

#include <vector>
#include <string>
#include "pl0_ast.hpp"

/**
 * All TAC format:
 *
 * a = b + c
 * a = 
 */

#ifdef TRACE
    #define __func__ __func__ << endl
#else
    #define __func__ ""
#endif

void pl0_tac_program(pl0_ast_program const *program);
void pl0_tac_prog(pl0_ast_prog const *prog);
void pl0_tac_const_stmt(pl0_ast_const_stmt const *stmts);
void pl0_tac_var_stmt(pl0_ast_var_stmt const *stmts);
void pl0_tac_procedure_stmt(pl0_ast_procedure_stmt const *stmts);
void pl0_tac_function_stmt(pl0_ast_function_stmt const *stmts);
void pl0_tac_procedure_header(pl0_ast_procedure_header const *header);
void pl0_tac_function_header(pl0_ast_function_header const *header);
void pl0_tac_param(pl0_ast_param_list const *param);
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
string pl0_tac_expr(pl0_ast_expression const *expr);
string pl0_tac_condition(pl0_ast_condtion const *cond);
string pl0_tac_term(pl0_ast_term const *term);
string pl0_tac_factor(pl0_ast_factor const * factor);
string pl0_tac_call_func(pl0_ast_call_func const *stmt);

extern struct IRBuilder irb;

void pl0_tac_program(pl0_ast_program const *program) {
    cout << __func__;
    irb.emit("begin program");
    pl0_tac_prog(program->program);
    irb.emit("end program");
}

void pl0_tac_prog(pl0_ast_prog const *prog) {
    cout << __func__;
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
}

void pl0_tac_const_stmt(pl0_ast_const_stmt const *stmts) {
    cout << __func__;
    if (!stmts) { return; }
    for (auto && def: stmts->stmt) {
        irb.emit("const " + def->id->id + ": " + std::to_string(def->val->val));
    }
    cout << __func__;

}

void pl0_tac_var_stmt(pl0_ast_var_stmt const *stmts) {
    cout << __func__;
    if (!stmts) { return; }
    for (auto && s: stmts->stmt) {
        for (auto && var: s->ids) {
            string t = s->type->type->type + (s->type->len == -1 ? "" : "array " + std::to_string(s->type->len));
            irb.emit("var " + var->id + ": " + t);
        }
    }
}

void pl0_tac_procedure_stmt(pl0_ast_procedure_stmt const *stmts) {
    cout << __func__;
    for (auto && proc: stmts->procs) {
        irb.emit("begin procedure");
        pl0_tac_procedure_header(proc.first);
        pl0_tac_prog(proc.second);
        irb.emit("end procedure");
    }
}

void pl0_tac_function_stmt(pl0_ast_function_stmt const *stmts) {
    cout << __func__;
    for (auto && func: stmts->funcs) {
        irb.emit("begin function");
        pl0_tac_function_header(func.first);
        pl0_tac_prog(func.second);
        irb.emit("end function");
    }
}

void pl0_tac_procedure_header(pl0_ast_procedure_header const *header) {
    cout << __func__;
    irb.emit("procedure " + header->id->id);
    pl0_tac_param(header->params);
}

void pl0_tac_function_header(pl0_ast_function_header const *header) {
    cout << __func__;
    irb.emit("function " + header->id->id + ": " + header->type->type);
    pl0_tac_param(header->params);
}

void pl0_tac_param(pl0_ast_param_list const *param) {
    cout << __func__;
    if (!param) { return; } // no parameter.
    for (auto && group: param->params) {
        for (auto && id: group->ids) {
            irb.emit("param " + group->type->type + " " + id->id);
        }
    }
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
    string val = pl0_tac_expr(stmt->val);
    if (stmt->idx) {
        string idx = pl0_tac_expr(stmt->idx);
        irb.emit(stmt->id->id + " [] " + idx + " = " + val);
    }
    else {
        irb.emit(stmt->id->id + " = " + val);
    }
}

void pl0_tac_cond_stmt(pl0_ast_cond_stmt const *stmt) {
    cout << __func__;
    string cond = pl0_tac_condition(stmt->cond);
    int thenlabel = irb.makelabel();
    int elselabel = irb.makelabel();
    int endlabel = irb.makelabel();
    irb.emit("iffalse cond goto L" + std::to_string(elselabel));
    irb.emitlabel(thenlabel);
    pl0_tac_stmt(stmt->then_block);
    irb.emit("goto L" + std::to_string(endlabel));
    irb.emitlabel(elselabel);
    pl0_tac_stmt(stmt->else_block);
    irb.emit("goto L" + std::to_string(endlabel));
    irb.emitlabel(endlabel);
}

void pl0_tac_case_stmt(pl0_ast_case_stmt const *stmt) {
    cout << "TODO" << __func__;
    // string val = pl0_tac_expr(stmt->expr);
    // int tblabel = irb.makelabel();
    // int endlabel = irb.makelabel();
    // irb.emit("switch " + val + " case " + std::to_string(endlabel));
    // std::vector<int> lbs;
    // for (auto item: cases)
    // irb.emit("end")
}

void pl0_tac_call_proc(pl0_ast_call_proc const *stmt) {
    cout << __func__;
    std::vector<std::string> args;
    for (auto argexpr: stmt->args->args) {
        args.emplace_back(pl0_tac_expr(argexpr->arg));
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
        irb.emit("read " + id->id);
    }
}

void pl0_tac_write_stmt(pl0_ast_write_stmt const *stmt) {
    cout << "TODO" << __func__;
}

void pl0_tac_null_stmt(pl0_ast_null_stmt const *stmt) {
    cout << __func__;
    irb.emit("null stmt");
}

string pl0_tac_expr(pl0_ast_expression const *expr) {
    cout << __func__;
    std::string prev = "", ans;
    for (auto term: expr->terms) {
        ans = irb.maketmp();
        irb.emit(ans + " = " + prev + string(1, term.first->op) + pl0_tac_term(term.second));
        prev = ans;
    }
    return ans;
}

string pl0_tac_condition(pl0_ast_condtion const *cond) {
    cout << __func__;
    std::string lhs = pl0_tac_expr(cond->lhs);
    std::string rhs = pl0_tac_expr(cond->rhs);
    string t = irb.maketmp();
    irb.emit(t + " = " + lhs + cond->op->op + rhs);
    return t;
}

string pl0_tac_term(pl0_ast_term const *term) {
    cout << __func__;
    std::string ans = pl0_tac_factor(term->factors[0].second);
    std::string prev = ans;
    if (term->factors.size() > (size_t)1) {
        for (size_t i = 1; i < term->factors.size(); ++i) {
            ans = irb.maketmp();
            irb.emit(ans + " = " + prev + string(1, term->factors[i].first->op) + pl0_tac_factor(term->factors[i].second));
            prev= ans;
        }
    }
    return ans;
}

string pl0_tac_factor(pl0_ast_factor const * factor) {
    cout << __func__;
    std::string ans;
    switch (factor->t) {
        case pl0_ast_factor::type_t::ID: ans = factor->ptr.id->id; break;
        case pl0_ast_factor::type_t::UNSIGNED: ans = std::to_string(factor->ptr.unsignedn->val); break;
        case pl0_ast_factor::type_t::EXPR: ans = pl0_tac_expr(factor->ptr.expr); break;
        case pl0_ast_factor::type_t::CALL_FUNC: ans = pl0_tac_call_func(factor->ptr.call_func); break;
        case pl0_ast_factor::type_t::ARRAY_E: ans = factor->arraye.first->id + " [] " + pl0_tac_expr(factor->arraye.second); break;
        default: ans = "ERROR"; cout << "UNIMPLEMENT EXPRESSION" << endl;
    }
    return ans;
}

string pl0_tac_call_func(pl0_ast_call_func const *stmt) {
    cout << __func__;
    std::vector<std::string> args;
    for (auto argexpr: stmt->args->args) {
        args.emplace_back(pl0_tac_expr(argexpr->arg));
    }
    for (auto arg: args) {
        irb.emit("push " + arg);
    }
    string retval = irb.makeret();
    irb.emit(retval);
    irb.emit("callproc " + stmt->fn->id);
    for (auto arg: args) {
        irb.emit("pop " + arg);
    }
    return retval;
}

#endif /* __PLO_TAC_GEN_HPP__ */


