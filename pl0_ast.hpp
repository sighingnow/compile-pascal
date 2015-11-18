
#include <string>
#include <vector>

struct pl0_ast_t;
struct pl0_ast_program;
struct pl0_ast_prog;
struct pl0_ast_const_stmt;
struct pl0_ast_const_define;
struct pl0_ast_constv;
struct pl0_ast_charseq;
struct pl0_ast_identify;
struct pl0_ast_var_stmt;
struct pl0_ast_var_define;
struct pl0_ast_type;
struct pl0_ast_primitive_type;
struct pl0_ast_executable;
struct pl0_ast_procedure_stmt;
struct pl0_ast_function_stmt;
struct pl0_ast_procedure_header;
struct pl0_ast_function_header;
struct pl0_ast_param_list;
struct pl0_ast_param_group;
struct pl0_ast_stmt;
struct pl0_ast_null_stmt;
struct pl0_ast_assign_stmt;
struct pl0_ast_function_id;
struct pl0_ast_expression;
struct pl0_ast_term;
struct pl0_ast_factor;
struct pl0_ast_call_func;
struct pl0_ast_arg_list;
struct pl0_ast_arg;
struct pl0_ast_addop;
struct pl0_ast_multop;
struct pl0_ast_condtion;
struct pl0_ast_comp_op;
struct pl0_ast_cond_stmt;
struct pl0_ast_case_stmt;
struct pl0_ast_case_term;
struct pl0_ast_for_stmt;
struct pl0_ast_call_proc;
struct pl0_ast_compound_stmt;
struct pl0_ast_read_stmt;
struct pl0_ast_write_stmt;
struct pl0_ast_alnum;

struct pl0_ast_program {
    struct pl0_ast_prog *program;
    pl0_ast_program(pl0_ast_prog *program): program(program) {}
};
struct pl0_ast_prog {
    struct pl0_ast_const_stmt *consts;
    struct pl0_ast_var_stmt *vars;
    std::vector<struct pl0_ast_executable *> exes;
    struct pl0_ast_compound_stmt *stmts;
    pl0_ast_prog() {}
    pl0_ast_prog(struct pl0_ast_const_stmt *consts,
        struct pl0_ast_var_stmt *vars,
        std::vector<struct pl0_ast_executable *> const & exes,
        struct pl0_ast_compound_stmt *stmts)
        : consts(consts), vars(vars), exes(exes), stmts(stmts) {
        cout << "parser prog." << endl;
    }
};
struct pl0_ast_const_stmt: pl0_ast_prog {
    std::vector<struct pl0_ast_const_define *> stmt;
    pl0_ast_const_stmt(std::vector<pl0_ast_const_define *> const & stmt): stmt(stmt) {} 
};
struct pl0_ast_const_define {
    struct pl0_ast_identify *id;
    struct pl0_ast_constv *val;
    pl0_ast_const_define(pl0_ast_identify *id, pl0_ast_constv *val): id(id), val(val) {}
};
struct pl0_ast_constv {
    int val;
    pl0_ast_constv(int const & val): val(val) {}
};
struct pl0_ast_charseq {
    std::string val;
    pl0_ast_charseq(std::string const & val): val(val) {}
};
struct pl0_ast_identify {
    std::string id;
    pl0_ast_identify(std::string const & id): id(id) {}
};
struct pl0_ast_var_stmt: pl0_ast_prog {
    std::vector<struct pl0_ast_var_define *> stmt;
    pl0_ast_var_stmt(std::vector<pl0_ast_var_define *> const & stmt): stmt(stmt) {}
};
struct pl0_ast_var_define {
    struct pl0_ast_type *type;
    std::vector<struct pl0_ast_identify*> ids;
    pl0_ast_var_define(pl0_ast_type *type, std::vector<pl0_ast_identify *> const & ids): type(type), ids(ids) {}
};
struct pl0_ast_type {
    struct pl0_ast_primitive_type *type;
    int len;
    pl0_ast_type(pl0_ast_primitive_type *type, int len): type(type), len(len) {}
};
struct pl0_ast_primitive_type {
    std::string type;
    pl0_ast_primitive_type(std::string type): type(type) {}
};
struct pl0_ast_executable {
    string const name = "pl0 executable object.";
    enum type_t {
        PROCEDURE, FUNCTION
    };
    type_t t;
    struct pl0_ast_procedure_stmt *proc;
    struct pl0_ast_function_stmt *func;
    pl0_ast_executable(pl0_ast_procedure_stmt *proc): t(type_t::PROCEDURE), proc(proc), func(nullptr) {}
    pl0_ast_executable(pl0_ast_function_stmt *func): t(type_t::FUNCTION), proc(nullptr), func(func) {}
};
struct pl0_ast_procedure_stmt {
    std::vector<std::pair<struct pl0_ast_procedure_header *, struct pl0_ast_prog *>> procs;
    pl0_ast_procedure_stmt(std::vector<std::pair<pl0_ast_procedure_header *, pl0_ast_prog *>> const & procs): procs(procs) {}
};
struct pl0_ast_function_stmt {
    std::vector<std::pair<struct pl0_ast_function_header *, struct pl0_ast_prog *>> funcs;
    pl0_ast_function_stmt(std::vector<std::pair<pl0_ast_function_header *, pl0_ast_prog *>> const & funcs): funcs(funcs) {}
};
struct pl0_ast_procedure_header {
    struct pl0_ast_identify *id;
    struct pl0_ast_param_list *params;
    pl0_ast_procedure_header(pl0_ast_identify *id, pl0_ast_param_list *params): id(id), params(params) {}
};
struct pl0_ast_function_header {
    struct pl0_ast_identify *id;
    struct pl0_ast_param_list *params;
    struct pl0_ast_primitive_type *type;
    pl0_ast_function_header(pl0_ast_identify *id, pl0_ast_param_list *params, pl0_ast_primitive_type *type): id(id), params(params), type(type) {}
};
struct pl0_ast_param_list {
    std::vector<struct pl0_ast_param_group *> params;
    pl0_ast_param_list(vector<pl0_ast_param_group *> const & params): params(params) {}
};
struct pl0_ast_param_group {
    std::vector<struct pl0_ast_identify *> ids;
    struct pl0_ast_primitive_type *type;
    pl0_ast_param_group(vector<pl0_ast_identify *> const & ids, pl0_ast_primitive_type *type): ids(ids), type(type) {}
};
struct pl0_ast_stmt {
    enum type_t {
        STMT, ASSIGN_STMT, COND_STMT, CASE_STMT, CALL_PROC, COMPOUND_STMT, WRITE_STMT, READ_STMT, FOR_STMT, NULL_STMT, INHERIT
    };
    type_t t;
    struct pl0_ast_stmt *stmt;
    pl0_ast_stmt(): t(type_t::INHERIT), stmt(nullptr) {}
    pl0_ast_stmt(type_t const t, pl0_ast_stmt *stmt): t(t), stmt(stmt) {}
};
struct pl0_ast_null_stmt: pl0_ast_stmt {
    pl0_ast_null_stmt(): pl0_ast_stmt(type_t::NULL_STMT, nullptr) {}
}; // empty statement, not in grammar.
struct pl0_ast_assign_stmt: pl0_ast_stmt {
    struct pl0_ast_identify *id;
    struct pl0_ast_expression *val;
    struct pl0_ast_expression *idx;
    pl0_ast_assign_stmt(pl0_ast_identify *id, pl0_ast_expression *val, pl0_ast_expression *idx): id(id), val(val), idx(idx) {}
};
struct pl0_ast_function_id {
    struct pl0_ast_identify *id;
    pl0_ast_function_id(pl0_ast_identify *id): id(id) {}
};
struct pl0_ast_expression {
    std::vector<std::pair<struct pl0_ast_addop *, struct pl0_ast_term *>> terms;
    pl0_ast_expression(std::vector<std::pair<struct pl0_ast_addop *, struct pl0_ast_term *>> const & terms): terms(terms) {}
};
struct pl0_ast_term {
    std::vector<std::pair<struct pl0_ast_multop *, struct pl0_ast_factor *>> factors;
    pl0_ast_term(std::vector<std::pair<pl0_ast_multop *, pl0_ast_factor *>> const & factors): factors(factors) {}
};
struct pl0_ast_factor {
    enum type_t {
        ID, UNSIGNED, EXPR, CALL_FUNC, ARRAY_E
    };
    type_t t;
    union {
        struct pl0_ast_identify *id;
        struct pl0_ast_unsigned *unsignedn;
        struct pl0_ast_expression *expr;
        struct pl0_ast_call_func *call_func;
    } ptr;
    std::pair<struct pl0_ast_identify *, struct pl0_ast_expression *> arraye; // for array access.
    pl0_ast_factor(type_t t, void *ptr): t(t) {
        switch (this->t) {
            case type_t::ID: this->ptr.id = static_cast<pl0_ast_identify *>(ptr);
            case type_t::EXPR: this->ptr.expr = static_cast<pl0_ast_expression *>(ptr);
            case type_t::CALL_FUNC: this->ptr.call_func = static_cast<pl0_ast_call_func *>(ptr);
            case type_t::UNSIGNED: this->ptr.unsignedn = static_cast<pl0_ast_unsigned *>(ptr);
            default: throw(this->t);
        }
    }
    pl0_ast_factor(std::pair<struct pl0_ast_identify *, struct pl0_ast_expression *> const & arraye): t(type_t::ARRAY_E), arraye(arraye) {}
};
struct pl0_ast_call_func {
    struct pl0_ast_identify *fn;
    struct pl0_ast_arg_list *args;
    pl0_ast_call_func(pair<pl0_ast_identify *, pl0_ast_arg_list *> const & call): fn(call.first), args(call.second) {}
};
struct pl0_ast_arg_list {
    std::vector<struct pl0_ast_arg *> args;
    pl0_ast_arg_list(std::vector<struct pl0_ast_arg *> const & args): args(args) {}
};
struct pl0_ast_arg {
    struct pl0_ast_expression *arg;
    pl0_ast_arg(pl0_ast_expression *arg): arg(arg) {}
};
struct pl0_ast_addop {
    enum {
        ADD, MINUS
    } OP;
    char op;
    pl0_ast_addop(char const & op): op(op) {}
};
struct pl0_ast_multop {
    enum {
        MULT, DIV
    } OP;
    char op;
    pl0_ast_multop(char const & op): op(op) {}
};
struct pl0_ast_condtion {
    struct pl0_ast_expression *lhs;
    struct pl0_ast_comp_op *op;
    struct pl0_ast_expression *rhs;
    pl0_ast_condtion(pl0_ast_expression *lhs, pl0_ast_comp_op *op, pl0_ast_expression *rhs): lhs(lhs), op(op), rhs(rhs) {}
};
struct pl0_ast_comp_op {
    enum {
        LT, LE, GT, GE, EQ, NE
    } OP;
    string op;
    pl0_ast_comp_op(string const & op): op(op) {}
};
struct pl0_ast_cond_stmt: pl0_ast_stmt {
    struct pl0_ast_condtion *cond;
    struct pl0_ast_stmt *then_block, *else_block;
    pl0_ast_cond_stmt(pl0_ast_condtion *cond, pl0_ast_stmt *then_block, pl0_ast_stmt *else_block): cond(cond), then_block(then_block), else_block(else_block) {}
};
struct pl0_ast_case_stmt: pl0_ast_stmt {
    struct pl0_ast_expression *expr;
    std::vector<pl0_ast_case_term *> terms;
    pl0_ast_case_stmt(pl0_ast_expression *expr, std::vector<pl0_ast_case_term *> const & terms): expr(expr), terms(terms) {}
};
struct pl0_ast_case_term {
    struct pl0_ast_constv *constv;
    struct pl0_ast_stmt *stmt;
    pl0_ast_case_term(pl0_ast_constv *constv, pl0_ast_stmt *stmt): constv(constv), stmt(stmt) {}
};
struct pl0_ast_for_stmt: pl0_ast_stmt {
    struct pl0_ast_identify *iter;
    struct pl0_ast_expression *initial, *end;
    struct pl0_ast_stmt *stmt;
    struct pl0_ast_constv *step;
    pl0_ast_for_stmt(pl0_ast_identify *iter, pl0_ast_expression *initial, pl0_ast_expression *end, pl0_ast_stmt *stmt, pl0_ast_constv *step): iter(iter), initial(initial), end(end), stmt(stmt), step(step) {}
};
struct pl0_ast_call_proc: pl0_ast_stmt {
    struct pl0_ast_identify *id;
    struct pl0_ast_arg_list *args;
    pl0_ast_call_proc(pl0_ast_identify *id, pl0_ast_arg_list *args): id(id), args(args) {}
};
struct pl0_ast_compound_stmt: pl0_ast_stmt, pl0_ast_prog {
    std::vector<struct pl0_ast_stmt *> stmt;
    pl0_ast_compound_stmt(std::vector<struct pl0_ast_stmt *> const & stmt): stmt(stmt) {}
};
struct pl0_ast_read_stmt: pl0_ast_stmt {
    std::vector<struct pl0_ast_identify *> ids;
    pl0_ast_read_stmt(std::vector<struct pl0_ast_identify *> const & ids): ids(ids) {}
};
struct pl0_ast_write_stmt: pl0_ast_stmt {
    enum type_t {
        ONLY_STRING,
        ONLY_EXPR,
        STRING_AND_EXPR
    };
    type_t t;
    struct pl0_ast_charseq *str;
    struct pl0_ast_expression *expr;
    pl0_ast_write_stmt(pl0_ast_charseq *str): t(type_t::ONLY_STRING), str(str) {}
    pl0_ast_write_stmt(pl0_ast_expression *expr): t(type_t::ONLY_EXPR), expr(expr) {}
    pl0_ast_write_stmt(pl0_ast_charseq *str, pl0_ast_expression *expr): t(type_t::STRING_AND_EXPR), str(str), expr(expr) {}
};
struct pl0_ast_alnum {
    char val;
    pl0_ast_alnum(char val): val(val) {}
};

