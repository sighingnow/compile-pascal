/**
 * All parsers.
 */

#include <iostream>
#include "input_t.hpp"
#include "parse_tool.hpp"
#include "pl0_ast.hpp"
using namespace std;

pair<int, pl0_ast_program *> pl0_program_fn(input_t *);
parser_t<pl0_ast_program *> pl0_program("pl0 program", pl0_program_fn);
pair<int, pl0_ast_prog *> pl0_prog_fn(input_t *);
parser_t<pl0_ast_prog *> pl0_prog("pl0 prog", pl0_prog_fn);

pair<int, pl0_ast_const_stmt *> pl0_const_stmt_fn(input_t *);
parser_t<pl0_ast_const_stmt *> pl0_const_stmt("pl0 const stmt", pl0_const_stmt_fn);
pair<int, pl0_ast_const_define *> pl0_const_define_fn(input_t *);
parser_t<pl0_ast_const_define *> pl0_const_define("pl0 const define", pl0_const_define_fn);
pair<int, pl0_ast_constv *> pl0_const_fn(input_t *);
parser_t<pl0_ast_constv *> pl0_const("pl0 const", pl0_const_fn);
pair<int, pl0_ast_constv *> pl0_char_fn(input_t *);
parser_t<pl0_ast_constv *> pl0_char("pl0 char", pl0_char_fn);
pair<int, pl0_ast_charseq *> pl0_charseq_fn(input_t *);
parser_t<pl0_ast_charseq *> pl0_charseq("pl0 charseq", pl0_charseq_fn);
pair<int, pl0_ast_constv *> pl0_unsigned_fn(input_t *);
parser_t<pl0_ast_constv *> pl0_unsigned("pl0 unsigned", pl0_unsigned_fn);

pair<int, pl0_ast_identify *> pl0_identify_fn(input_t *);
parser_t<pl0_ast_identify *> pl0_identify("pl0 identify", pl0_identify_fn);
pair<int, pl0_ast_var_stmt *> pl0_var_stmt_fn(input_t *);
parser_t<pl0_ast_var_stmt *> pl0_var_stmt("pl0 vardesc stmt", pl0_var_stmt_fn);
pair<int, pl0_ast_var_define *> pl0_var_define_fn(input_t *);
parser_t<pl0_ast_var_define *> pl0_var_define("pl0 vardesc", pl0_var_define_fn);

pair<int, pl0_ast_type *> pl0_type_fn(input_t *);
parser_t<pl0_ast_type *> pl0_type("pl0 type", pl0_type_fn);
pair<int, pl0_ast_primitive_type *> pl0_primitive_type_fn(input_t *);
parser_t<pl0_ast_primitive_type *> pl0_primitive_type("pl0 primitive type", pl0_primitive_type_fn);

pair<int, pl0_ast_executable *> pl0_executable_fn(input_t *);
parser_t<pl0_ast_executable *> pl0_executable("pl0 executable, function and procedure", pl0_executable_fn);
pair<int, pl0_ast_procedure_stmt *> pl0_procedure_stmt_fn(input_t *);
parser_t<pl0_ast_procedure_stmt *> pl0_procedure_stmt("pl0 procedure stmt", pl0_procedure_stmt_fn);
pair<int, pl0_ast_function_stmt *> pl0_function_stmt_fn(input_t *);
parser_t<pl0_ast_function_stmt *> pl0_function_stmt("pl0 function stmt", pl0_function_stmt_fn);
pair<int, pl0_ast_procedure_header *> pl0_procedure_header_fn(input_t *);
parser_t<pl0_ast_procedure_header *> pl0_procedure_header("pl0 procedure header", pl0_procedure_header_fn);
pair<int, pl0_ast_function_header *> pl0_function_header_fn(input_t *);
parser_t<pl0_ast_function_header *> pl0_function_header("pl0 function header", pl0_function_header_fn);

pair<int, pl0_ast_param_list *> pl0_param_list_fn(input_t *);
parser_t<pl0_ast_param_list *> pl0_param_list("pl0 param list", pl0_param_list_fn);
pair<int, pl0_ast_param_group *> pl0_param_group_fn(input_t *);
parser_t<pl0_ast_param_group *> pl0_param_group("pl0 param group", pl0_param_group_fn);

pair<int, pl0_ast_stmt *> pl0_stmt_fn(input_t *);
parser_t<pl0_ast_stmt *> pl0_stmt("pl0 stmt", pl0_stmt_fn);
pair<int, pl0_ast_null_stmt *> pl0_null_stmt_fn(input_t *);
parser_t<pl0_ast_null_stmt *> pl0_null_stmt("pl0 null stmt", pl0_null_stmt_fn); // empty statement, not in grammar.
pair<int, pl0_ast_assign_stmt *> pl0_assign_stmt_fn(input_t *);
parser_t<pl0_ast_assign_stmt *> pl0_assign_stmt("pl0 assign stmt", pl0_assign_stmt_fn);

pair<int, pl0_ast_function_id *> pl0_function_id_fn(input_t *);
parser_t<pl0_ast_function_id *> pl0_function_id("pl0 function id", pl0_function_id_fn);
pair<int, pl0_ast_expression *> pl0_expression_fn(input_t *);
parser_t<pl0_ast_expression *> pl0_expression("pl0 expression", pl0_expression_fn);
pair<int, pl0_ast_term *> pl0_term_fn(input_t *);
parser_t<pl0_ast_term *> pl0_term("pl0 term", pl0_term_fn);
pair<int, pl0_ast_factor *> pl0_factor_fn(input_t *);
parser_t<pl0_ast_factor *> pl0_factor("pl0 factor", pl0_factor_fn);
pair<int, pl0_ast_call_func *> pl0_call_func_fn(input_t *);
parser_t<pl0_ast_call_func *> pl0_call_func("pl0 call func", pl0_call_func_fn);
pair<int, pl0_ast_arg_list *> pl0_arg_list_fn(input_t *);
parser_t<pl0_ast_arg_list *> pl0_arg_list("pl0 arg list", pl0_arg_list_fn);
pair<int, pl0_ast_arg *> pl0_arg_fn(input_t *);
parser_t<pl0_ast_arg *> pl0_arg("pl0 arg", pl0_arg_fn);

pair<int, pl0_ast_addop *> pl0_addop_fn(input_t *);
parser_t<pl0_ast_addop *> pl0_addop("pl0 addop", pl0_addop_fn);
pair<int, pl0_ast_multop *> pl0_multop_fn(input_t *);
parser_t<pl0_ast_multop *> pl0_multop("pl0 multop", pl0_multop_fn);

pair<int, pl0_ast_condtion *> pl0_condition_fn(input_t *);
parser_t<pl0_ast_condtion *> pl0_condition("pl0 condition", pl0_condition_fn);
pair<int, pl0_ast_comp_op *> pl0_comp_op_fn(input_t *);
parser_t<pl0_ast_comp_op *> pl0_comp_op("pl0 comp op", pl0_comp_op_fn);

pair<int, pl0_ast_cond_stmt *> pl0_cond_stmt_fn(input_t *);
parser_t<pl0_ast_cond_stmt *> pl0_cond_stmt("pl0 cond stmt", pl0_cond_stmt_fn);
pair<int, pl0_ast_case_stmt *> pl0_case_stmt_fn(input_t *);
parser_t<pl0_ast_case_stmt *> pl0_case_stmt("pl0 case stmt", pl0_case_stmt_fn);
pair<int, pl0_ast_case_term *> pl0_case_term_fn(input_t *);
parser_t<pl0_ast_case_term *> pl0_case_term("pl0 case term", pl0_case_term_fn);
pair<int, pl0_ast_for_stmt *> pl0_for_stmt_fn(input_t *);
parser_t<pl0_ast_for_stmt *> pl0_for_stmt("pl0 for stmt", pl0_for_stmt_fn);
pair<int, pl0_ast_call_proc *> pl0_call_proc_fn(input_t *);
parser_t<pl0_ast_call_proc *> pl0_call_proc("pl0 call proc", pl0_call_proc_fn);
pair<int, pl0_ast_compound_stmt *> pl0_compound_stmt_fn(input_t *);
parser_t<pl0_ast_compound_stmt *> pl0_compound_stmt("pl0 compound stmt", pl0_compound_stmt_fn);
pair<int, pl0_ast_read_stmt *> pl0_read_stmt_fn(input_t *);
parser_t<pl0_ast_read_stmt *> pl0_read_stmt("pl0 read stmt", pl0_read_stmt_fn);
pair<int, pl0_ast_write_stmt *> pl0_write_stmt_fn(input_t *);
parser_t<pl0_ast_write_stmt *> pl0_write_stmt("pl0 write stmt", pl0_write_stmt_fn);

pair<int, pl0_ast_alnum *> pl0_alpha_fn(input_t *);
parser_t<pl0_ast_alnum *> pl0_alpha("pl0 alpha", pl0_alpha_fn);
pair<int, pl0_ast_alnum *> pl0_digit_fn(input_t *);
parser_t<pl0_ast_alnum *> pl0_digit("pl0 digit", pl0_digit_fn);

/* IR builder. */

std::string Value::str() {
    switch (t) {
        case INT: return std::to_string(iv);
        case STR: return sv;
    }
}

std::string TAC::str() {
    return op + " " + (rd ? rd->str() : "") + " " + (rs ? rs->str() : "") + " " + (rt ? rt->str() : "");
}

int IRBuilder::makelabel() {
    return this->label++;
}
string IRBuilder::maketmp() {
    return "~t" + std::to_string(temp++);
}
string IRBuilder::makeret() {
    return "~ret" + std::to_string(ret++);
}
void IRBuilder::dump() {
    for (auto ir: irs) {
        cout << ";; " << ir.str() << endl;
    }
}

struct IRBuilder irb;

/* Global env. (symbol table) */


variable::variable(std::string name, std::string type = "integer"): name(name), type(type), len(-1) {}
variable::variable(std::string name, std::string type, int len): name(name), type(type), len(len) {}
bool operator == (variable const & a, variable const & b) {
    return a.name == b.name && a.type == b.type;
}

constant::constant(std::string name, int v): name(name), val(v) {}
bool operator == (constant const & a, constant const & b) {
    return a.name == b.name;
}

proc::proc(std::string name, std::vector<std::string> & param_t): name(name), param_t(param_t) {}
bool operator == (proc const & a, proc const & b) {
    bool ans = a.name == b.name && a.param_t.size() == b.param_t.size();
    if (ans && a.param_t.size() > 0) {
        for (size_t i = 0; i < a.param_t.size(); ++i) {
            ans = ans && a.param_t[i] == b.param_t[i];
        }
    }
    return ans;
}

func::func(std::string name, std::string rettype, std::vector<std::string> & param_t): name(name), rettype(rettype), param_t(param_t) {}
bool operator == (func const & a, func const & b) {
    bool ans = a.name == b.name && a.rettype == b.rettype && a.param_t.size() == b.param_t.size();
    if (ans && a.param_t.size() > 0) {
        for (size_t i = 0; i < a.param_t.size(); ++i) {
            ans = ans && a.param_t[i] == b.param_t[i];
        }
    }
    return ans;
}

/* pl0_env */



