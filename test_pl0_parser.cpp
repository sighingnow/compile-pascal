#include <string>
#include <fstream>
#include <streambuf>

#include "gtest/gtest.h"

#include "input_t.hpp"
#include "parse_tool.hpp"

#include "pl0_ast.hpp"

using namespace std;

extern parser_t<pl0_ast_program *> pl0_program;
extern parser_t<pl0_ast_prog *> pl0_prog;
extern parser_t<pl0_ast_const_stmt *> pl0_const_stmt;
extern parser_t<pl0_ast_const_define *> pl0_const_define;
extern parser_t<pl0_ast_constv *> pl0_const;
extern parser_t<pl0_ast_constv *> pl0_char;
extern parser_t<pl0_ast_charseq *> pl0_charseq;
extern parser_t<pl0_ast_constv *> pl0_unsigned;
extern parser_t<pl0_ast_identify *> pl0_identify;
extern parser_t<pl0_ast_var_stmt *> pl0_var_stmt;
extern parser_t<pl0_ast_var_define *> pl0_var_define;
extern parser_t<pl0_ast_type *> pl0_type;
extern parser_t<pl0_ast_primitive_type *> pl0_primitive_type;
extern parser_t<pl0_ast_executable *> pl0_executable;
extern parser_t<pl0_ast_procedure_stmt *> pl0_procedure_stmt;
extern parser_t<pl0_ast_function_stmt *> pl0_function_stmt;
extern parser_t<pl0_ast_procedure_header *> pl0_procedure_header;
extern parser_t<pl0_ast_function_header *> pl0_function_header;
extern parser_t<pl0_ast_param_list *> pl0_param_list;
extern parser_t<pl0_ast_param_group *> pl0_param_group;
extern parser_t<pl0_ast_stmt *> pl0_stmt;
extern parser_t<pl0_ast_null_stmt *> pl0_null_stmt; // empty statement, not in grammar.
extern parser_t<pl0_ast_assign_stmt *> pl0_assign_stmt;
extern parser_t<pl0_ast_function_id *> pl0_function_id;
extern parser_t<pl0_ast_expression *> pl0_expression;
extern parser_t<pl0_ast_term *> pl0_term;
extern parser_t<pl0_ast_factor *> pl0_factor;
extern parser_t<pl0_ast_call_func *> pl0_call_func;
extern parser_t<pl0_ast_arg_list *> pl0_arg_list;
extern parser_t<pl0_ast_arg *> pl0_arg;
extern parser_t<pl0_ast_addop *> pl0_addop;
extern parser_t<pl0_ast_multop *> pl0_multop;
extern parser_t<pl0_ast_condtion *> pl0_condition;
extern parser_t<pl0_ast_comp_op *> pl0_comp_op;
extern parser_t<pl0_ast_cond_stmt *> pl0_cond_stmt;
extern parser_t<pl0_ast_case_stmt *> pl0_case_stmt;
extern parser_t<pl0_ast_case_term *> pl0_case_term;
extern parser_t<pl0_ast_for_stmt *> pl0_for_stmt;
extern parser_t<pl0_ast_call_proc *> pl0_call_proc;
extern parser_t<pl0_ast_compound_stmt *> pl0_compound_stmt;
extern parser_t<pl0_ast_read_stmt *> pl0_read_stmt;
extern parser_t<pl0_ast_write_stmt *> pl0_write_stmt;
extern parser_t<pl0_ast_alnum *> pl0_alpha;
extern parser_t<pl0_ast_alnum *> pl0_digit;

// Test parser for PL0 grammar.

TEST(PL0Parser, pl0_digit) {
    auto parse_tool = ParsecT<decltype(pl0_digit)>(pl0_digit);

    EXPECT_EQ(parse_tool(new input_t("1234")).actual->val, '1');
    EXPECT_EQ(parse_tool(new input_t("a")).status, false);
}

TEST(PL0Parser, pl0_alpha) {
    auto parse_tool = ParsecT<decltype(pl0_alpha)>(pl0_alpha);

    EXPECT_EQ(parse_tool(new input_t("abcd")).status, true);
    EXPECT_EQ(parse_tool(new input_t("ABCD")).actual->val, 'A');
    EXPECT_EQ(parse_tool(new input_t("")).status, false);
    EXPECT_EQ(parse_tool(new input_t("1")).status, false);
}

TEST(PL0Parser, pl0_char) {
    auto parse_tool = ParsecT<decltype(pl0_char)>(pl0_char);

    auto res1 = parse_tool(new input_t("'a''b'"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual->val, 'a');

    auto res2 = parse_tool(new input_t("'ab"));
    EXPECT_EQ(res2.status, false);

    auto res3 = parse_tool(new input_t("a'"));
    EXPECT_EQ(res3.status, false);
}

TEST(PL0Parser, pl0_charseq) {
    auto parse_tool = ParsecT<decltype(pl0_charseq)>(pl0_charseq);

    auto res1 = parse_tool(new input_t(string(1, '"') + "a bc de " + string(1, '"')));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual->val, "a bc de ");
    EXPECT_EQ(res1.len, 10);
}

TEST(PL0Parser, pl0_unsigned) {
    auto parse_tool = ParsecT<decltype(pl0_unsigned)>(pl0_unsigned);

    auto res1 = parse_tool(new input_t("123456"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual->val, 123456);
}

TEST(PL0Parser, pl0_const) {
    auto parse_tool = ParsecT<decltype(pl0_const)>(pl0_const);

    auto res1 = parse_tool(new input_t("  +1245"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual->val, 1245);

    auto res2 = parse_tool(new input_t("-12345  "));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual->val, -12345);

    auto res3 = parse_tool(new input_t("12345"));
    EXPECT_EQ(res3.status, true);
    EXPECT_EQ(res3.actual->val, 12345);

    auto res4 = parse_tool(new input_t("'c'"));
    EXPECT_EQ(res4.status, true);
    EXPECT_EQ(res4.actual->val, (int)'c');

    EXPECT_EQ(parse_tool(new input_t("c")).status, false);
    EXPECT_EQ(parse_tool(new input_t("+'c'")).status, false);
}

TEST(PL0Parser, pl0_identify) {
    auto parse_tool = ParsecT<decltype(pl0_identify)>(pl0_identify);

    auto res1 = parse_tool(new input_t("integer "));
    EXPECT_EQ(res1.status, false);

    auto res2 = parse_tool(new input_t("charc;"));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual->id, "charc");

    EXPECT_EQ(parse_tool(new input_t("xxxxxxxxxxxxx")).status, true);
    EXPECT_EQ(parse_tool(new input_t("xxxxxxxxxxxxx=")).actual->id, "xxxxxxxxxxxxx");
}

TEST(PL0Parser, pl0_primitive_type) {
    auto parse_tool = ParsecT<decltype(pl0_primitive_type)>(pl0_primitive_type);

    auto res1 = parse_tool(new input_t("integer"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual->type, "integer");

    auto res2 = parse_tool(new input_t("char"));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual->type, "char");

    EXPECT_EQ(parse_tool(new input_t("xxxxxxxxxxxxx")).status, false);
}

TEST(PL0Parser, pl0_type) {
    auto parse_tool = ParsecT<decltype(pl0_type)>(pl0_type);

    auto res1 = parse_tool(new input_t("integer; const;"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual->type->type, "integer");

    auto res2 = parse_tool(new input_t("array [124] of integer"));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual->type->type, "integer");
    EXPECT_EQ(res2.actual->len, 124);

    auto res3 = parse_tool(new input_t("array [  0 ] of char"));
    EXPECT_EQ(res3.status, true);
    EXPECT_EQ(res3.actual->type->type, "char");
    EXPECT_EQ(res3.actual->len, 0);
    EXPECT_EQ(res3.len, 20);
}

TEST(PL0Parser, pl0_var_define) {
    auto parse_tool = ParsecT<decltype(pl0_var_define)>(pl0_var_define);

    auto res1 = parse_tool(new input_t("x, y, z: integer;"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual->type->type->type, "integer");
    EXPECT_EQ(res1.actual->ids.size(), (size_t)3);

    auto res2 = parse_tool(new input_t("k, x, y: array [177] of char"));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual->type->type->type, "char");
    EXPECT_EQ(res2.actual->type->len, 177);
    EXPECT_EQ(res2.actual->ids[1]->id, "x");

    auto res3 = parse_tool(new input_t("x, y, z: array [177] off char"));
    EXPECT_EQ(res3.status, false);
}

TEST(PL0Parser, pl0_var_stmt) {
    auto parse_tool = ParsecT<decltype(pl0_var_stmt)>(pl0_var_stmt);

    auto res1 = parse_tool(new input_t("var x, y, z: integer;"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual->stmt[0]->type->type->type, "integer");
    EXPECT_EQ(res1.actual->stmt[0]->ids.size(), (size_t)3);

    auto res2 = parse_tool(new input_t("var k, x, y: array [177] of char; kk, xx, yy: integer;"));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual->stmt[0]->type->type->type, "char");
    EXPECT_EQ(res2.actual->stmt[0]->type->len, 177);
    EXPECT_EQ(res2.actual->stmt[0]->ids[1]->id, "x");
    EXPECT_EQ(res2.actual->stmt[1]->type->type->type, "integer");
    EXPECT_EQ(res2.actual->stmt[1]->type->len, -1);
    EXPECT_EQ(res2.actual->stmt[1]->ids[1]->id, "xx");
    EXPECT_EQ(res2.actual->stmt[1]->ids.size(), (size_t)3);

    auto res3 = parse_tool(new input_t("var x, y, z: array [177] of char"));
    EXPECT_EQ(res3.status, false);
}

TEST(PL0Parser, pl0_const_define) {
    auto parse_tool = ParsecT<decltype(pl0_const_define)>(pl0_const_define);

    auto res1 = parse_tool(new input_t("x = +1245"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual->id->id, "x");
    EXPECT_EQ(res1.actual->val->val, 1245);

    auto res2 = parse_tool(new input_t("tt=-12345"));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual->id->id, "tt");
    EXPECT_EQ(res2.actual->val->val, -12345);

    auto res3 = parse_tool(new input_t("c = 'c'"));
    EXPECT_EQ(res3.status, true);
    EXPECT_EQ(res3.actual->id->id, "c");
    EXPECT_EQ(res3.actual->val->val, (int)'c');

    EXPECT_EQ(parse_tool(new input_t("c = c")).status, false);
    EXPECT_EQ(parse_tool(new input_t("c = -'c'")).status, false);
}

TEST(PL0Parser, pl0_const_stmt) {
    auto parse_tool = ParsecT<decltype(pl0_const_stmt)>(pl0_const_stmt);

    auto res1 = parse_tool(new input_t("const x = +1245, t = -12456, u = 'c';"));
    EXPECT_TRUE(res1.status);
    EXPECT_EQ(res1.actual->stmt[0]->id->id, "x");
    EXPECT_EQ(res1.actual->stmt[0]->val->val, 1245);

    auto res2 = parse_tool(new input_t("const tt=-12345, x ='c', z = 12;"));
    EXPECT_TRUE(res2.status);
    EXPECT_EQ(res2.actual->stmt.size(), (size_t)3);
    EXPECT_EQ(res2.actual->stmt[0]->id->id, "tt");
    EXPECT_EQ(res2.actual->stmt[2]->val->val, 12);

    auto res3 = parse_tool(new input_t("const zero = '0', minus = '-';")); // '-' is not a valid <字母>
    EXPECT_FALSE(res3.status);

    EXPECT_FALSE(parse_tool(new input_t("const c = 't'")).status);
}

TEST(PL0Parser, ReadStmt) {
    auto parse_tool = ParsecT<decltype(pl0_read_stmt)>(pl0_read_stmt);

    EXPECT_EQ(parse_tool(new input_t("  read(  aaaa)")).status, true);
    EXPECT_EQ(parse_tool(new input_t("  read(  aaaa, ddd, x, eee, ddd)")).status, true);
}

TEST(PL0Parser, WriteStmt) {
    auto parse_tool = ParsecT<decltype(pl0_write_stmt)>(pl0_write_stmt);

    EXPECT_EQ(parse_tool(new input_t("  write(  aaaa)")).status, true);
    EXPECT_EQ(parse_tool(new input_t("  write(\"fsdfsdf\")")).status, true);
    EXPECT_EQ(parse_tool(new input_t("  write(\"fsdfsdf\", aaaaa)")).status, true);
}

TEST(PL0Parser, ConditionStmt) {
    auto parse_tool = ParsecT<decltype(pl0_cond_stmt)>(pl0_cond_stmt);

    EXPECT_TRUE(parse_tool(new input_t("if 1 <> 2 then write(\"abcd\")")).status);
    EXPECT_TRUE(parse_tool(new input_t("if 1 <> 2 then write(\"abcd\") else write(\"1234\")")).status);
    EXPECT_FALSE(parse_tool(new input_t("if 1 <> 2 then write(\"abcd\") else ")).status);
    // test dangling else.
    auto res = parse_tool(new input_t("if 1 <> 2 then if 3=3 then write(\"abcd\") else write(\"1234\")"));
    EXPECT_TRUE(res.status);
    EXPECT_EQ(res.actual->else_block, nullptr);
    EXPECT_NE(dynamic_cast<pl0_ast_cond_stmt *>(res.actual->then_block)->else_block, nullptr);
}

TEST(PL0Parser, AssignStmt) {
    auto parse_tool = ParsecT<decltype(pl0_assign_stmt)>(pl0_assign_stmt);

    EXPECT_TRUE(parse_tool(new input_t("a := 100")).status);
    EXPECT_TRUE(parse_tool(new input_t("a := -b+c-c*d-e/f+a")).status);
    EXPECT_TRUE(parse_tool(new input_t("a[100] := b+c-c*d-e/f+a")).status);
    EXPECT_TRUE(parse_tool(new input_t("a[3] := a['3']+c['e']-c*d-e/f+a")).status);
    EXPECT_FALSE(parse_tool(new input_t("a[3] := a['3']+c['e']-c*d-e/f+a")).strict);
    EXPECT_TRUE(parse_tool(new input_t("a[-1] := b+c-c*d-e/f+a")).strict);
}

TEST(PL0Parser, ForStmt) {
    auto parse_tool = ParsecT<decltype(pl0_for_stmt)>(pl0_for_stmt);

    EXPECT_TRUE(parse_tool(new input_t("for i := 10 downto 1 do write(i)")).status);
    EXPECT_TRUE(parse_tool(new input_t("for k := j to j do begin write(\"k is: \", k) end")).status);
    auto res = parse_tool(new input_t("for k := 1 to 100 do write(\"fdsf\");"));
    EXPECT_TRUE(res.status);
    EXPECT_FALSE(res.strict);
}

TEST(PL0Parser, Stmt) {
    auto parse_tool = ParsecT<decltype(pl0_stmt)>(pl0_stmt);

    EXPECT_TRUE(parse_tool(new input_t("  write(  aaaa)")).status);
}

TEST(PL0Parser, CompoundStmt) {
    auto parse_tool = ParsecT<decltype(pl0_compound_stmt)>(pl0_compound_stmt);

    EXPECT_EQ(parse_tool(new input_t("begin write(aaaa) end")).status, true);
    EXPECT_EQ(parse_tool(new input_t("begin write(\"fsdfsdf\") end")).status, true);
    EXPECT_EQ(parse_tool(new input_t("begin read(aaaa); write(aaaa) end")).status, true);
    EXPECT_EQ(parse_tool(new input_t("begin read(aaaa); write(\"fsdfsd\") end")).status, true);
    EXPECT_EQ(parse_tool(new input_t("begin read(aaaa); write(aaaa); write(\"fsdfsd\") end")).status, true);
    EXPECT_EQ(parse_tool(new input_t("begin read(aaaa); write(aaaa); write(\"fsdfsd\"); end")).status, true);
    EXPECT_EQ(parse_tool(new input_t("begin read(aaaa); write(aaaa) write(\"fsdfsd\") end")).status, false);
}

input_t *load_case(char const *fname) {
    std::ifstream t(fname);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return new input_t(str);
}

TEST(PL0Parser, FunctionStmt) {
    auto parse_tool = ParsecT<decltype(pl0_function_stmt)>(pl0_function_stmt);

    auto res1 = parse_tool(new input_t("function f(var a, v: char): char; begin write(aaaa) end;")); 
    EXPECT_TRUE(res1.status);
    EXPECT_TRUE(res1.strict);
}

TEST(PL0Parser, ExecutableStmt) {
    auto parse_tool = ParsecT<decltype(pl0_executable)>(pl0_executable);

    auto res1 = parse_tool(new input_t("function f(var a, v: char): char; begin write(aaaa) end;")); 
    EXPECT_TRUE(res1.status);
    EXPECT_TRUE(res1.strict);
}

TEST(PL0Parser, Simple) {
    auto parse_tool = ParsecT<decltype(pl0_program)>(pl0_program);

    auto res1 = parse_tool(load_case("pl0_cases/simple1.pas"));
    EXPECT_TRUE(res1.status);
    EXPECT_TRUE(res1.strict);
    auto res2 = parse_tool(load_case("pl0_cases/simple2.pas"));
    EXPECT_TRUE(res2.status);
    EXPECT_TRUE(res2.strict);
    auto res3 = parse_tool(load_case("pl0_cases/simple3.pas"));
    EXPECT_TRUE(res3.status);
    EXPECT_TRUE(res3.strict);
}

TEST(PL0Parser, Function) {
    auto parse_tool = ParsecT<decltype(pl0_program)>(pl0_program);

    auto res1 = parse_tool(load_case("pl0_cases/function1.pas"));
    EXPECT_TRUE(res1.status);
    EXPECT_TRUE(res1.strict);
    auto res2 = parse_tool(load_case("pl0_cases/function2.pas"));
    EXPECT_TRUE(res2.status);
    EXPECT_TRUE(res2.strict);
    auto res3 = parse_tool(load_case("pl0_cases/function_swap.pas"));
    EXPECT_TRUE(res3.status);
    EXPECT_TRUE(res3.strict);
    auto res4 = parse_tool(load_case("pl0_cases/function_gcd.pas"));
    EXPECT_TRUE(res4.status);
    EXPECT_TRUE(res4.strict);
}


