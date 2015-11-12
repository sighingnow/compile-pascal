#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parsec.hpp"

TEST_CASE("Test all primitive parsers form characters.") {
    SECTION("any character") {
        auto parser = any;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        string s1 = "abcdeABCDE12345";
        auto res1 = parse_tool(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'a');
    }

    SECTION("blank character") {
        auto parser = blank;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        string s1 = " x";
        auto res1 = parse_tool(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == ' ');
    }

    SECTION("blank character invalid") {
        auto parser = blank;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto s1 = "x";
        auto res1 = parse_tool(s1);
        REQUIRE(res1.status == false);   
    }

    SECTION("single specified character") {
        auto parser = character('a');
        auto parse_tool = ParsecT<decltype(parser)>(parser);
        
        auto s1 = "abcde";
        auto res1 = parse_tool(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'a');

        auto s2 = "x";
        auto res2 = parse_tool(s2);
        REQUIRE(res2.status == false);
    }

    SECTION("string literal") {
        auto parser = string_literal("abcde");
        auto parse_tool = ParsecT<decltype(parser)>(parser);
        
        auto s1 = "abcdeabcde";
        auto res1 = parse_tool(s1);
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == "abcde");

        auto s2 = "x";
        auto res2 = parse_tool(s2);
        REQUIRE(res2.status == false);
    }

    SECTION("test spaces") {
        auto parser = spaces;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        string s1 = "          ";
        REQUIRE(parse_tool(s1).len == (int)s1.length());

        string s2 = "     abcd";
        REQUIRE(parse_tool(s2).len == 5);
    }
}

TEST_CASE("Parser combinators.") {
    SECTION("Compose two parsers, ignore the result of the first parser, and return the second one's.") {
        auto parser = character('x') >> character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool("xy");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'y');

        auto res2 = parse_tool("x");
        REQUIRE(res2.status == false);

        auto res3 = parse_tool("y");
        REQUIRE(res3.status == false);
    }

    SECTION("Choice one from two parsers.") {
        auto parser = character('x') | character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);
        
        auto res1 = parse_tool("xy");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'x');

        auto res2 = parse_tool("yx");
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == 'y');

        auto res3 = parse_tool("zzz");
        REQUIRE(res3.status == false);
    }

    SECTION("Joint two parsers into one, aggregate their result into one as the final result.") {
        auto parser = character('x') + character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool("xy");
        REQUIRE(res1.status == true);
        REQUIRE(res1.len == 2);
        REQUIRE(res1.actual.first == 'x');
        REQUIRE(res1.actual.second == 'y');
    }

    SECTION("Skip specified parser.") {
        auto parser = character('x') << character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool("xy");
        REQUIRE(res1.status == true);
        REQUIRE(res1.len == 2);
        REQUIRE(res1.actual == 'x');   

        auto res2 = parse_tool("xz");
        REQUIRE(res2.status == false);
    }

    SECTION("Combine skip combinator and joint combinator.") {
        auto parser = (character('x') << character('y')) + (character('a') << character('b'));
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool("xyab");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual.first == 'x');
        REQUIRE(res1.actual.second == 'a');
        REQUIRE(res1.len == 4);

        auto res2 = parse_tool("xzab");
        REQUIRE(res2.status == false);

        auto res3 = parse_tool("xyac");
        REQUIRE(res3.status == false);
    }

    SECTION("Except combinator.") {
        auto parser = (character('x') | character('y') | character('z')) - character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool("x");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'x');

        auto res2 = parse_tool("y");
        REQUIRE(res2.status == false);

        auto res3 = parse_tool("z");
        REQUIRE(res3.status == true);
        REQUIRE(res3.actual == 'z');
    }
}

TEST_CASE("Repeat combinators.") {

    function<string (vector<char>)> fn = [](vector<char> const & vec) {
        string ans; for (auto c: vec) { ans.push_back(c); } return ans;
    };

    SECTION("Test times combinator.") {
        auto parser = times<decltype(character('x'))>(character('x'), 2, 10) / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool("xxx");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == "xxx");

        auto res2 = parse_tool("xxxxxxxxxxxxxxxx");
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "xxxxxxxxxx");

        auto res3 = parse_tool("xxxxk");
        REQUIRE(res3.status == true);
        REQUIRE(res3.actual == "xxxx");
        REQUIRE(res3.len == 4);

        auto res4 = parse_tool("x");
        REQUIRE(res4.status == false);
    }

    SECTION("Test repeat 0 time using times combinator.") {
        auto parser = times<decltype(character('x'))>(character('x'), 0, 0) / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool("xxx");
        REQUIRE(res1.status == true);
        REQUIRE(res1.len == 0);

        auto res2 = parse_tool("");
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "");
    }

    SECTION("Test many.") {
        auto parser = ++any / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        REQUIRE(parse_tool("x").actual == "x");
        REQUIRE(parse_tool("x y z").actual == "x y z");
        REQUIRE(parse_tool("").status == true);
    }

    SECTION("Test many1.") {
        auto parser = any++ / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);
        
        REQUIRE(parse_tool("x").actual == "x");
        REQUIRE(parse_tool("x y z").actual == "x y z");
        REQUIRE(parse_tool("").status == false);
    }

    SECTION("Test count.") {
        auto parser = any * 5 / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);
        
        REQUIRE(parse_tool("x").status == false);
        REQUIRE(parse_tool("x y z   z").actual == "x y z");
        REQUIRE(parse_tool("123456").actual == "12345");
    }

    SECTION("Test sepby1.") {
        auto parser = (character('x') % character('y')) / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool("xyxyxyx");
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == "xxxx");
        REQUIRE(res1.len == 7);


        auto res2 = parse_tool("xyxyxyxy");
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "xxxx");
        REQUIRE(res2.len == 7);
    }
}


