#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parsec.hpp"

TEST_CASE("Test all primitive parsers form characters.") {
    SECTION("any character") {
        auto s1 = StreamT<string, char>("text string", "abcdeABCDE12345");
        auto res1 = any.parse(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.actual == 'a');
    }

    SECTION("blank character") {
        auto s1 = StreamT<string, char>("text string", " x");
        auto res1 = any.parse(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.actual == ' ');

        auto s2 = StreamT<string, char>("text string", "x");
        auto res2 = blank.parse(s2);
        REQUIRE(res2.status == false);
    }

    SECTION("single specified character") {
        auto s1 = StreamT<string, char>("text string", "abcde");
        auto res1 = character('a').parse(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.actual == 'a');

        auto s2 = StreamT<string, char>("text string", "x");
        auto res2 = character('a').parse(s2);
        REQUIRE(res2.status == false);
    }

    SECTION("string literal") {
        auto s1 = StreamT<string, char>("text string", "abcdeabcde");
        auto res1 = string_literal("abcde").parse(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.actual == "abcde");

        auto s2 = StreamT<string, char>("text string", "x");
        auto res2 = blank.parse(s2);
        REQUIRE(res2.status == false);
    }
}


