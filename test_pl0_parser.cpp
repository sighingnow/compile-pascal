#include <string>
#include <fstream>
#include <streambuf>

#include "gtest/gtest.h"
#include "pl0_parser.hpp"

// Test parser for PL0 grammar.

// TEST(PL0Parser, pl0_digit) {
//     auto parse_tool = ParsecT<decltype(pl0_digit)>(pl0_digit);

//     EXPECT_EQ(parse_tool(new input_t("1234")).actual->val, '1');
//     EXPECT_EQ(parse_tool(new input_t("a")).status, false);
// }

// TEST(PL0Parser, pl0_alpha) {
//     auto parse_tool = ParsecT<decltype(pl0_alpha)>(pl0_alpha);

//     EXPECT_EQ(parse_tool(new input_t("abcd")).status, true);
//     EXPECT_EQ(parse_tool(new input_t("ABCD")).actual->val, 'A');
//     EXPECT_EQ(parse_tool(new input_t("")).status, false);
//     EXPECT_EQ(parse_tool(new input_t("1")).status, false);
// }

// TEST(PL0Parser, pl0_char) {
//     auto parse_tool = ParsecT<decltype(pl0_char)>(pl0_char);

//     auto res1 = parse_tool(new input_t("'a''b'"));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->val, 'a');

//     auto res2 = parse_tool(new input_t("'ab"));
//     EXPECT_EQ(res2.status, false);

//     auto res3 = parse_tool(new input_t("a'"));
//     EXPECT_EQ(res3.status, false);
// }

// TEST(PL0Parser, pl0_charseq) {
//     auto parse_tool = ParsecT<decltype(pl0_charseq)>(pl0_charseq);

//     auto res1 = parse_tool(new input_t(string(1, '"') + "a bc de " + string(1, '"')));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->val, "a bc de ");
//     EXPECT_EQ(res1.len, 10);
// }

// TEST(PL0Parser, pl0_unsigned) {
//     auto parse_tool = ParsecT<decltype(pl0_unsigned)>(pl0_unsigned);

//     auto res1 = parse_tool(new input_t("123456"));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->val, 123456);
// }

// TEST(PL0Parser, pl0_const) {
//     auto parse_tool = ParsecT<decltype(pl0_const)>(pl0_const);

//     auto res1 = parse_tool(new input_t("  +1245"));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->val, 1245);

//     auto res2 = parse_tool(new input_t("-12345  "));
//     EXPECT_EQ(res2.status, true);
//     EXPECT_EQ(res2.actual->val, -12345);

//     auto res3 = parse_tool(new input_t("12345"));
//     EXPECT_EQ(res3.status, true);
//     EXPECT_EQ(res3.actual->val, 12345);

//     auto res4 = parse_tool(new input_t("'c'"));
//     EXPECT_EQ(res4.status, true);
//     EXPECT_EQ(res4.actual->val, (int)'c');

//     EXPECT_EQ(parse_tool(new input_t("c")).status, false);
//     EXPECT_EQ(parse_tool(new input_t("+'c'")).status, false);
// }

// TEST(PL0Parser, pl0_identify) {
//     auto parse_tool = ParsecT<decltype(pl0_identify)>(pl0_identify);

//     auto res1 = parse_tool(new input_t("integer "));
//     EXPECT_EQ(res1.actual->id, "integer");
//     EXPECT_EQ(res1.status, true);

//     auto res2 = parse_tool(new input_t("char;"));
//     EXPECT_EQ(res2.status, true);
//     EXPECT_EQ(res2.actual->id, "char");

//     EXPECT_EQ(parse_tool(new input_t("xxxxxxxxxxxxx")).status, true);
//     EXPECT_EQ(parse_tool(new input_t("xxxxxxxxxxxxx=")).actual->id, "xxxxxxxxxxxxx");
// }

// TEST(PL0Parser, pl0_primitive_type) {
//     auto parse_tool = ParsecT<decltype(pl0_primitive_type)>(pl0_primitive_type);

//     auto res1 = parse_tool(new input_t("integer"));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->type, "integer");

//     auto res2 = parse_tool(new input_t("char"));
//     EXPECT_EQ(res2.status, true);
//     EXPECT_EQ(res2.actual->type, "char");

//     EXPECT_EQ(parse_tool(new input_t("xxxxxxxxxxxxx")).status, false);
// }

// TEST(PL0Parser, pl0_type) {
//     auto parse_tool = ParsecT<decltype(pl0_type)>(pl0_type);

//     auto res1 = parse_tool(new input_t("integer; const;"));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->type->type, "integer");

//     auto res2 = parse_tool(new input_t("array [124] of integer"));
//     EXPECT_EQ(res2.status, true);
//     EXPECT_EQ(res2.actual->type->type, "integer");
//     EXPECT_EQ(res2.actual->len, 124);

//     auto res3 = parse_tool(new input_t("array [  0 ] of char"));
//     EXPECT_EQ(res3.status, true);
//     EXPECT_EQ(res3.actual->type->type, "char");
//     EXPECT_EQ(res3.actual->len, 0);
//     EXPECT_EQ(res3.len, 20);
// }

// TEST(PL0Parser, pl0_var_define) {
//     auto parse_tool = ParsecT<decltype(pl0_var_define)>(pl0_var_define);

//     auto res1 = parse_tool(new input_t("x, y, z: integer;"));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->type->type->type, "integer");
//     EXPECT_EQ(res1.actual->ids.size(), (size_t)3);

//     auto res2 = parse_tool(new input_t("k, x, y: array [177] of char"));
//     EXPECT_EQ(res2.status, true);
//     EXPECT_EQ(res2.actual->type->type->type, "char");
//     EXPECT_EQ(res2.actual->type->len, 177);
//     EXPECT_EQ(res2.actual->ids[1]->id, "x");

//     auto res3 = parse_tool(new input_t("x, y, z: array [177] off char"));
//     EXPECT_EQ(res3.status, false);
// }

// TEST(PL0Parser, pl0_var_stmt) {
//     auto parse_tool = ParsecT<decltype(pl0_var_stmt)>(pl0_var_stmt);

//     auto res1 = parse_tool(new input_t("var x, y, z: integer;"));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->stmt[0]->type->type->type, "integer");
//     EXPECT_EQ(res1.actual->stmt[0]->ids.size(), (size_t)3);

//     auto res2 = parse_tool(new input_t("var k, x, y: array [177] of char; kk, xx, yy: integer;"));
//     EXPECT_EQ(res2.status, true);
//     EXPECT_EQ(res2.actual->stmt[0]->type->type->type, "char");
//     EXPECT_EQ(res2.actual->stmt[0]->type->len, 177);
//     EXPECT_EQ(res2.actual->stmt[0]->ids[1]->id, "x");
//     EXPECT_EQ(res2.actual->stmt[1]->type->type->type, "integer");
//     EXPECT_EQ(res2.actual->stmt[1]->type->len, -1);
//     EXPECT_EQ(res2.actual->stmt[1]->ids[1]->id, "xx");
//     EXPECT_EQ(res2.actual->stmt[1]->ids.size(), (size_t)3);

//     auto res3 = parse_tool(new input_t("var x, y, z: array [177] of char"));
//     EXPECT_EQ(res3.status, false);
// }

// TEST(PL0Parser, pl0_const_define) {
//     auto parse_tool = ParsecT<decltype(pl0_const_define)>(pl0_const_define);

//     auto res1 = parse_tool(new input_t("x = +1245"));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->id->id, "x");
//     EXPECT_EQ(res1.actual->val->val, 1245);

//     auto res2 = parse_tool(new input_t("tt=-12345"));
//     EXPECT_EQ(res2.status, true);
//     EXPECT_EQ(res2.actual->id->id, "tt");
//     EXPECT_EQ(res2.actual->val->val, -12345);

//     auto res3 = parse_tool(new input_t("c = 'c'"));
//     EXPECT_EQ(res3.status, true);
//     EXPECT_EQ(res3.actual->id->id, "c");
//     EXPECT_EQ(res3.actual->val->val, (int)'c');

//     EXPECT_EQ(parse_tool(new input_t("c = c")).status, false);
//     EXPECT_EQ(parse_tool(new input_t("c = -'c'")).status, false);
// }

// TEST(PL0Parser, pl0_const_stmt) {
//     auto parse_tool = ParsecT<decltype(pl0_const_stmt)>(pl0_const_stmt);

//     auto res1 = parse_tool(new input_t("const x = +1245, t = -12456, u = 'c';"));
//     EXPECT_EQ(res1.status, true);
//     EXPECT_EQ(res1.actual->stmt[0]->id->id, "x");
//     EXPECT_EQ(res1.actual->stmt[0]->val->val, 1245);

//     auto res2 = parse_tool(new input_t("const tt=-12345, x ='c', z = 12;"));
//     EXPECT_EQ(res2.status, true);
//     EXPECT_EQ(res2.actual->stmt.size(), (size_t)3);
//     EXPECT_EQ(res2.actual->stmt[0]->id->id, "tt");
//     EXPECT_EQ(res2.actual->stmt[2]->val->val, 12);

//     EXPECT_EQ(parse_tool(new input_t("const c = 't'")).status, false);
// }

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

TEST(PL0Parser, Stmt) {
    auto parse_tool = ParsecT<decltype(pl0_stmt)>(pl0_stmt);

    EXPECT_EQ(parse_tool(new input_t("  write(  aaaa)")).status, true);
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

TEST(PL0Parser, Simple) {
    auto parse_tool = ParsecT<decltype(pl0_program)>(pl0_program);

    EXPECT_EQ(parse_tool(load_case("pl0_cases/simple1.pas")).status, true);
    EXPECT_EQ(parse_tool(load_case("pl0_cases/simple2.pas")).status, true);
    EXPECT_EQ(parse_tool(load_case("pl0_cases/simple3.pas")).status, true);
    EXPECT_EQ(parse_tool(load_case("pl0_cases/simple4.pas")).status, true);
    EXPECT_EQ(parse_tool(load_case("pl0_cases/simple5.pas")).status, true);
}

