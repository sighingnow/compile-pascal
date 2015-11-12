#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "pl0_parser.hpp"

TEST_CASE("Test parser for PL0 grammar.") {
    SECTION("Parse pl0_digit") {
        auto parse_tool = ParsecT<decltype(pl0_digit)>(pl0_digit);

        REQUIRE(parse_tool("1234").actual == '1');
        REQUIRE(parse_tool("a").status == false);
    }

    SECTION("Parse pl0_alpha") {
        auto parse_tool = ParsecT<decltype(pl0_alpha)>(pl0_alpha);

        REQUIRE(parse_tool("abcd").status == true);
        REQUIRE(parse_tool("ABCD").actual == 'A');
        REQUIRE(parse_tool("").status == false);
        REQUIRE(parse_tool("1").status == false);
    }

    SECTION("Parse pl0_char") {
        auto parse_tool = ParsecT<decltype(pl0_char)>(pl0_char);

        auto res1 = parse_tool("'a''b'");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'a');

        auto res2 = parse_tool("'ab");
        REQUIRE(res2.status == false);

        auto res3 = parse_tool("a'");
        REQUIRE(res3.status == false);
    }

    SECTION("Parse pl0_charseq") {
        auto parse_tool = ParsecT<decltype(pl0_charseq)>(pl0_charseq);

        auto res1 = parse_tool(string(1, '"') + "a bc de " + string(1, '"'));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == "a bc de ");
        REQUIRE(res1.len == 10);
    }

    SECTION("Parse pl0_unsigned") {
        auto parse_tool = ParsecT<decltype(pl0_unsigned)>(pl0_unsigned);

        auto res1 = parse_tool("123456");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 123456);
    }

    SECTION("Parse pl0_const") {
        auto parse_tool = ParsecT<decltype(pl0_const)>(pl0_const);

        auto res1 = parse_tool("+1245");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 1245);

        auto res2 = parse_tool("-12345");
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == -12345);

        auto res3 = parse_tool("12345");
        REQUIRE(res3.status == true);
        REQUIRE(res3.actual == 12345);

        auto res4 = parse_tool("'c'");
        REQUIRE(res4.status == true);
        REQUIRE(res4.actual == (int)'c');

        REQUIRE(parse_tool("c").status == false);
        REQUIRE(parse_tool("+'c'").status == false);
    }

    SECTION("Parse pl0_identify") {
        auto parse_tool = ParsecT<decltype(pl0_identify)>(pl0_identify);

        auto res1 = parse_tool("integer ");
        REQUIRE(res1.actual == "integer");
        REQUIRE(res1.status == true);

        auto res2 = parse_tool("char;");
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "char");

        REQUIRE(parse_tool("xxxxxxxxxxxxx").status == true);
    }

    SECTION("Parse pl0_primitive_type") {
        auto parse_tool = ParsecT<decltype(pl0_primitive_type)>(pl0_primitive_type);

        auto res1 = parse_tool("integer");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == "integer");

        auto res2 = parse_tool("char");
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "char");

        REQUIRE(parse_tool("xxxxxxxxxxxxx").status == false);
    }

    SECTION("Parse pl0_type") {
        auto parse_tool = ParsecT<decltype(pl0_type)>(pl0_type);

        auto res1 = parse_tool("integer; const;");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual.first == "integer");

        auto res2 = parse_tool("array [124] of integer");
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual.first == "integer");
        REQUIRE(res2.actual.second == 124);

        auto res3 = parse_tool("array [  0 ] of char");
        REQUIRE(res3.status == true);
        REQUIRE(res3.actual.first == "char");
        REQUIRE(res3.actual.second == 0);
        REQUIRE(res3.len == 20);
    }
}





