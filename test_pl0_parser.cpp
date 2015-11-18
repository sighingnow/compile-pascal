// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"
#include "pl0_parser.hpp"

// TEST_CASE("Test parser for PL0 grammar.") {
//     SECTION("Parse pl0_digit") {
//         auto parse_tool = ParsecT<decltype(pl0_digit)>(pl0_digit);

//         REQUIRE(parse_tool("1234").actual == '1');
//         REQUIRE(parse_tool("a").status == false);
//     }

//     SECTION("Parse pl0_alpha") {
//         auto parse_tool = ParsecT<decltype(pl0_alpha)>(pl0_alpha);

//         REQUIRE(parse_tool("abcd").status == true);
//         REQUIRE(parse_tool("ABCD").actual == 'A');
//         REQUIRE(parse_tool("").status == false);
//         REQUIRE(parse_tool("1").status == false);
//     }

//     SECTION("Parse pl0_char") {
//         auto parse_tool = ParsecT<decltype(pl0_char)>(pl0_char);

//         auto res1 = parse_tool("'a''b'");
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual == 'a');

//         auto res2 = parse_tool("'ab");
//         REQUIRE(res2.status == false);

//         auto res3 = parse_tool("a'");
//         REQUIRE(res3.status == false);
//     }

//     SECTION("Parse pl0_charseq") {
//         auto parse_tool = ParsecT<decltype(pl0_charseq)>(pl0_charseq);

//         auto res1 = parse_tool(string(1, '"') + "a bc de " + string(1, '"'));
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual == "a bc de ");
//         REQUIRE(res1.len == 10);
//     }

//     SECTION("Parse pl0_unsigned") {
//         auto parse_tool = ParsecT<decltype(pl0_unsigned)>(pl0_unsigned);

//         auto res1 = parse_tool("123456");
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual == 123456);
//     }

//     SECTION("Parse pl0_const") {
//         auto parse_tool = ParsecT<decltype(pl0_const)>(pl0_const);

//         auto res1 = parse_tool("  +1245");
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual == 1245);

//         auto res2 = parse_tool("-12345  ");
//         REQUIRE(res2.status == true);
//         REQUIRE(res2.actual == -12345);

//         auto res3 = parse_tool("12345");
//         REQUIRE(res3.status == true);
//         REQUIRE(res3.actual == 12345);

//         auto res4 = parse_tool("'c'");
//         REQUIRE(res4.status == true);
//         REQUIRE(res4.actual == (int)'c');

//         REQUIRE(parse_tool("c").status == false);
//         REQUIRE(parse_tool("+'c'").status == false);
//     }

//     SECTION("Parse pl0_identify") {
//         auto parse_tool = ParsecT<decltype(pl0_identify)>(pl0_identify);

//         auto res1 = parse_tool("integer ");
//         REQUIRE(res1.actual == "integer");
//         REQUIRE(res1.status == true);

//         auto res2 = parse_tool("char;");
//         REQUIRE(res2.status == true);
//         REQUIRE(res2.actual == "char");

//         REQUIRE(parse_tool("xxxxxxxxxxxxx").status == true);
//         REQUIRE(parse_tool("xxxxxxxxxxxxx=").actual == "xxxxxxxxxxxxx");
//     }

//     SECTION("Parse pl0_primitive_type") {
//         auto parse_tool = ParsecT<decltype(pl0_primitive_type)>(pl0_primitive_type);

//         auto res1 = parse_tool("integer");
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual == "integer");

//         auto res2 = parse_tool("char");
//         REQUIRE(res2.status == true);
//         REQUIRE(res2.actual == "char");

//         REQUIRE(parse_tool("xxxxxxxxxxxxx").status == false);
//     }

//     SECTION("Parse pl0_type") {
//         auto parse_tool = ParsecT<decltype(pl0_type)>(pl0_type);

//         auto res1 = parse_tool("integer; const;");
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual.first == "integer");

//         auto res2 = parse_tool("array [124] of integer");
//         REQUIRE(res2.status == true);
//         REQUIRE(res2.actual.first == "integer");
//         REQUIRE(res2.actual.second == 124);

//         auto res3 = parse_tool("array [  0 ] of char");
//         REQUIRE(res3.status == true);
//         REQUIRE(res3.actual.first == "char");
//         REQUIRE(res3.actual.second == 0);
//         REQUIRE(res3.len == 20);
//     }

//     SECTION("Parse pl0_vardesc") {
//         auto parse_tool = ParsecT<decltype(pl0_vardesc)>(pl0_vardesc);

//         auto res1 = parse_tool("x, y, z: integer;");
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual.second.first == "integer");
//         REQUIRE(res1.actual.first.size() == 3);

//         auto res2 = parse_tool("k, x, y: array [177] of char");
//         REQUIRE(res2.status == true);
//         REQUIRE(res2.actual.second.first == "char");
//         REQUIRE(res2.actual.second.second == 177);
//         REQUIRE(res2.actual.first[1] == "x");

//         auto res3 = parse_tool("x, y, z: array [177] off char");
//         REQUIRE(res3.status == false);
//     }

//     SECTION("Parse pl0_vardesc_stmt") {
//         auto parse_tool = ParsecT<decltype(pl0_vardesc_stmt)>(pl0_vardesc_stmt);

//         auto res1 = parse_tool("var x, y, z: integer;");
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual[0].second.first == "integer");
//         REQUIRE(res1.actual[0].first.size() == 3);

//         auto res2 = parse_tool("var k, x, y: array [177] of char; kk, xx, yy: integer;");
//         REQUIRE(res2.status == true);
//         REQUIRE(res2.actual[0].second.first == "char");
//         REQUIRE(res2.actual[0].second.second == 177);
//         REQUIRE(res2.actual[0].first[1] == "x");
//         REQUIRE(res2.actual[1].second.first == "integer");
//         REQUIRE(res2.actual[1].second.second == -1);
//         REQUIRE(res2.actual[1].first[1] == "xx");
//         REQUIRE(res2.actual[1].first.size() == 3);

//         auto res3 = parse_tool("var x, y, z: array [177] of char");
//         REQUIRE(res3.status == false);
//     }

//     SECTION("Parse pl0_const_define") {
//         auto parse_tool = ParsecT<decltype(pl0_const_define)>(pl0_const_define);

//         auto res1 = parse_tool("x = +1245");
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual.first == "x");
//         REQUIRE(res1.actual.second == 1245);

//         auto res2 = parse_tool("tt=-12345");
//         REQUIRE(res2.status == true);
//         REQUIRE(res2.actual.first == "tt");
//         REQUIRE(res2.actual.second == -12345);

//         auto res3 = parse_tool("c = 'c'");
//         REQUIRE(res3.status == true);
//         REQUIRE(res3.actual.first == "c");
//         REQUIRE(res3.actual.second == (int)'c');

//         REQUIRE(parse_tool("c = c").status == false);
//         REQUIRE(parse_tool("c = -'c'").status == false);
//     }

//     SECTION("Parse pl0_const_desc") {
//         auto parse_tool = ParsecT<decltype(pl0_const_stmt)>(pl0_const_stmt);

//         auto res1 = parse_tool("const x = +1245, t = -12456, u = 'c';");
//         REQUIRE(res1.status == true);
//         REQUIRE(res1.actual[0].first == "x");
//         REQUIRE(res1.actual[0].second == 1245);

//         auto res2 = parse_tool("const tt=-12345, x ='c', z = 12;");
//         REQUIRE(res2.status == true);
//         REQUIRE(res2.actual.size() == 3);
//         REQUIRE(res2.actual[1].first == "x");
//         REQUIRE(res2.actual[2].second == 12);

//         REQUIRE(parse_tool("const c = 't'").status == false);
//     }

// }





