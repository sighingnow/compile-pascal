#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parsec.hpp"

using S = StreamT<string, char>; // type alias for string stream contains characters.

TEST_CASE("Test all primitive parsers form characters.") {
    SECTION("any character") {
        auto s1 = S("text string", "abcdeABCDE12345");
        auto res1 = any.parse(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.actual == 'a');
    }

    SECTION("blank character") {
        auto s1 = S("text string", " x");
        auto res1 = any.parse(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.actual == ' ');

        auto s2 = S("text string", "x");
        auto res2 = blank.parse(s2);
        REQUIRE(res2.status == false);
    }

    SECTION("single specified character") {
        auto s1 = S("text string", "abcde");
        auto res1 = character('a').parse(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.actual == 'a');

        auto s2 = S("text string", "x");
        auto res2 = character('a').parse(s2);
        REQUIRE(res2.status == false);
    }

    SECTION("string literal") {
        auto s1 = S("text string", "abcdeabcde");
        auto res1 = string_literal("abcde").parse(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.actual == "abcde");

        auto s2 = S("text string", "x");
        auto res2 = blank.parse(s2);
        REQUIRE(res2.status == false);
    }
}

TEST_CASE("Parser combinators.") {
    SECTION("Compose two parsers, ignore the result of the first parser, and return the second one's.") {
        auto parser = compose<S, char, char, char>(character('x'), character('y'));

        auto res1 = parser.parse(S("string xy", "xy"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.actual == 'y');

        auto res2 = parser.parse(S("string x", "x"));
        REQUIRE(res2.status == false);

        auto res3 = parser.parse(S("string y", "y"));
        REQUIRE(res3.status == false);
    }

    SECTION("Choice one from two parsers.") {
        auto parser = choice<S, char, char, char>(character('x'), character('y'));

        auto res1 = parser.parse(S("string xy", "xy"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'x');

        auto res2 = parser.parse(S("string yx", "yx"));
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == 'y');

        auto res3 = parser.parse(S("string zzz", "zzz"));
        REQUIRE(res3.status == false);
    }

    SECTION("Joint two parsers into one, aggregate their result into one as the final result.") {
        auto parser = joint<S, char, char, string>(character('x'), character('y'));

        auto res1 = parser.parse(S("string xy", "xy"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.len == 2);
        REQUIRE(res1.actual == "xy");
    }

    SECTION("Skip specified parser.") {
        auto parser = skip<S, char, char, char>(character('x'), character('y'));

        auto res1 = parser.parse(S("string xy", "xy"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.loc == make_pair(1, 1));
        REQUIRE(res1.len == 2);
        REQUIRE(res1.actual == 'x');   

        auto res2 = parser.parse(S("string xz", "xz"));
        REQUIRE(res2.status == false);
    }

    SECTION("Combine skip combinator and joint combinator.") {
        auto parser = joint<S, char, char, string>(
            skip<S, char, char, char>(character('x'), character('y')),
            skip<S, char, char, char>(character('a'), character('b')));

        auto res1 = parser.parse(S("string xyab", "xyab"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == "xa");
        REQUIRE(res1.len == 4);

        auto res2 = parser.parse(S("string xzab", "xzab"));
        REQUIRE(res2.status == false);

        auto res3 = parser.parse(S("string xyac", "xyac"));
        REQUIRE(res3.status == false);
    }
}

TEST_CASE("Repeat combinators.") {
    SECTION("Test times combinator.") {
        auto parser = times<S, char, char, string>(character('x'), 2, 10);

        auto res1 = parser.parse(S("string xxx", "xxx"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == "xxx");

        auto res2 = parser.parse(S("string xxxxxxxxxxxxxxxx", "xxxxxxxxxxxxxxxx"));
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "xxxxxxxxxx");

        auto res3 = parser.parse(S("string xxxxk", "xxxxk"));
        REQUIRE(res3.status == true);
        REQUIRE(res3.actual == "xxxx");
        REQUIRE(res3.len == 4);

        auto res4 = parser.parse(S("string shorter than 2, x", "x"));
        REQUIRE(res4.status == false);
    }

    SECTION("Test repeat 0 time using times combinator.") {
        auto parser = times<S, char, char, string>(character('x'), 0, 0);

        auto res1 = parser.parse(S("string xxx", "xxx"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.len == 0);

        auto res2 = parser.parse(S("string contains no character", ""));
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "");
    }

    SECTION("Test many.") {
        auto parser = many<S, char, char, string>(any);

        REQUIRE(parser.parse(S("string x", "x")).actual == "x");
        REQUIRE(parser.parse(S("string x y z", "x y z")).actual == "x y z");
        REQUIRE(parser.parse(S("string contains no character.", "")).status == true);
    }

    SECTION("Test many1.") {
        auto parser = many1<S, char, char, string>(any);

        REQUIRE(parser.parse(S("string x", "x")).actual == "x");
        REQUIRE(parser.parse(S("string x y z", "x y z")).actual == "x y z");
        REQUIRE(parser.parse(S("string contains no character.", "")).status == false);
    }

    SECTION("Test count.") {
        auto parser = countk<S, char, char, string>(any, 5);

        REQUIRE(parser.parse(S("string x", "x")).status == false);
        REQUIRE(parser.parse(S("string x y z   z", "x y z   z")).actual == "x y z");
        REQUIRE(parser.parse(S("string contains no character.", "123456")).actual == "12345");
    }
}









