#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parsec.hpp"

TEST_CASE("Test all primitive parsers form characters.") {
    // SECTION("any character") {
    //     auto parser = any;
    //     auto parse_tool = ParsecT<decltype(parser)>(parser);

    //     input_t *s1 = new input_t("abcdeABCDE12345");
    //     auto res1 = parse_tool(s1);
    //     REQUIRE(res1.status == true);
    //     REQUIRE(res1.actual == 'a');
    // }

    // SECTION("blank character") {
    //     auto parser = blank;
    //     auto parse_tool = ParsecT<decltype(parser)>(parser);

    //     input_t *s1 = new input_t(" x");
    //     auto res1 = parse_tool(s1);
    //     REQUIRE(res1.status == true);
    //     REQUIRE(res1.actual == ' ');
    // }

    // SECTION("blank character invalid") {
    //     auto parser = blank;
    //     auto parse_tool = ParsecT<decltype(parser)>(parser);

    //     input_t *s1 = new input_t("x");
    //     auto res1 = parse_tool(s1);
    //     REQUIRE(res1.status == false);   
    // }

    // SECTION("single specified character") {
    //     auto parser = character('a');
    //     auto parse_tool = ParsecT<decltype(parser)>(parser);
        
    //     input_t *s1 = new input_t("abcde");
    //     auto res1 = parse_tool(s1);
    //     REQUIRE(res1.status == true);
    //     REQUIRE(res1.actual == 'a');

    //     input_t *s2 = new input_t("x");
    //     auto res2 = parse_tool(s2);
    //     REQUIRE(res2.status == false);
    // }

    // SECTION("string literal") {
    //     auto parser = string_literal("abcde");
    //     auto parse_tool = ParsecT<decltype(parser)>(parser);
        
    //     input_t *s1 = new input_t("abcdeabcde");
    //     auto res1 = parse_tool(s1);
    //     REQUIRE(res1.status == true);
    //     REQUIRE(res1.actual == "abcde");

    //     input_t *s2 = new input_t("x");
    //     auto res2 = parse_tool(s2);
    //     REQUIRE(res2.status == false);
    // }
}

TEST_CASE("Parser combinators.") {
    SECTION("Compose two parsers, ignore the result of the first parser, and return the second one's.") {
        auto parser = character('x') >> character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool(new input_t("xy"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'y');

        auto res2 = parse_tool(new input_t("x"));
        REQUIRE(res2.status == false);

        auto res3 = parse_tool(new input_t("y"));
        REQUIRE(res3.status == false);
    }

    SECTION("Choice one from two parsers.") {
        auto parser = character('x') | character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);
        
        auto res1 = parse_tool(new input_t("xy"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'x');

        auto res2 = parse_tool(new input_t("yx"));
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == 'y');

        auto res3 = parse_tool(new input_t("zzz"));
        REQUIRE(res3.status == false);
    }

    SECTION("Joint two parsers into one, aggregate their result into one as the final result.") {
        auto parser = character('x') + character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool(new input_t("xy"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.len == 2);
        REQUIRE(res1.actual.first == 'x');
        REQUIRE(res1.actual.second == 'y');
    }

    SECTION("Skip specified parser.") {
        auto parser = character('x') << character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool(new input_t("xy"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.len == 2);
        REQUIRE(res1.actual == 'x');

        auto res2 = parse_tool(new input_t("xz"));
        REQUIRE(res2.status == false);
    }

    SECTION("Combine skip combinator and joint combinator.") {
        auto parser = (character('x') << character('y')) + (character('a') << character('b'));
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool(new input_t("xyab"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual.first == 'x');
        REQUIRE(res1.actual.second == 'a');
        REQUIRE(res1.len == 4);

        auto res2 = parse_tool(new input_t("xzab"));
        REQUIRE(res2.status == false);

        auto res3 = parse_tool(new input_t("xyac"));
        REQUIRE(res3.status == false);
    }

    SECTION("Ends with combinator and compose combinator.") {
        auto parser = (spaces >> string_literal("abcd") < character('a')) >> string_literal("abcd") < character(',');
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res = parse_tool(new input_t("abcdabcd,"));
        REQUIRE(res.status == true);
        REQUIRE(res.actual == "abcd");
        REQUIRE(res.len == 8);
    }

    SECTION("Except combinator.") {
        auto parser = (character('x') | character('y') | character('z')) - character('y');
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool(new input_t("x"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == 'x');

        auto res2 = parse_tool(new input_t("y"));
        REQUIRE(res2.status == false);

        auto res3 = parse_tool(new input_t("z"));
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

        auto res1 = parse_tool(new input_t("xxx"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == "xxx");

        auto res2 = parse_tool(new input_t("xxxxxxxxxxxxxxxx"));
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "xxxxxxxxxx");

        auto res3 = parse_tool(new input_t("xxxxk"));
        REQUIRE(res3.status == true);
        REQUIRE(res3.actual == "xxxx");
        REQUIRE(res3.len == 4);

        auto res4 = parse_tool(new input_t("x"));
        REQUIRE(res4.status == false);
    }

    SECTION("Test optional combinator.") {
        auto parser = (~(++digit)) + (~string_literal("abcde"));
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool(new input_t("abcdec"));
        auto out1 = res1.actual;
        REQUIRE(res1.status == true);
        REQUIRE(out1.first.size() == 0);
        REQUIRE(out1.second == "abcde");

        auto res2 = parse_tool(new input_t("525435d"));
        auto out2 = res2.actual;
        REQUIRE(res2.status == true);
        REQUIRE(out2.first[2] == '5');
        REQUIRE(out2.second == "");
    }

    SECTION("Test repeat 0 time using times combinator.") {
        auto parser = times<decltype(character('x'))>(character('x'), 0, 0) / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool(new input_t("xxx"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.len == 0);

        auto res2 = parse_tool(new input_t(""));
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "");
    }

    SECTION("Test many.") {
        auto parser = ++any / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        REQUIRE(parse_tool(new input_t("x")).actual == "x");
        REQUIRE(parse_tool(new input_t("x y z")).actual == "x y z");
        REQUIRE(parse_tool(new input_t("")).status == true);
    }

    SECTION("test spaces") {
        auto parser = spaces;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        input_t *s1 = new input_t("          ");
        REQUIRE(parse_tool(s1).len == (int)s1->length());

        input_t *s2 = new input_t("     abcd");
        REQUIRE(parse_tool(s2).len == 5);
    }

    SECTION("Test many1.") {
        auto parser = any++ / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);
        
        REQUIRE(parse_tool(new input_t("x")).actual == "x");
        REQUIRE(parse_tool(new input_t("x y z")).actual == "x y z");
        REQUIRE(parse_tool(new input_t("")).status == false);
    }

    SECTION("Test count.") {
        auto parser = any * 5 / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);
        
        REQUIRE(parse_tool(new input_t("x")).status == false);
        REQUIRE(parse_tool(new input_t("x y z   z")).actual == "x y z");
        REQUIRE(parse_tool(new input_t("123456")).actual == "12345");
    }

    SECTION("Test sepby1.") {
        auto parser = (character('x') % character('y')) / fn;
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool(new input_t("xyxyxyx"));
        REQUIRE(res1.status == true);
        REQUIRE(res1.actual == "xxxx");
        REQUIRE(res1.len == 7);

        auto res2 = parse_tool(new input_t("xyxyxyxy"));
        REQUIRE(res2.status == true);
        REQUIRE(res2.actual == "xxxx");
        REQUIRE(res2.len == 7);
    }
}


TEST_CASE("Chain combinator for recursive grammars.") {
    SECTION("Test left recursive combinator.") {
        auto parser = (character('x') | character('y') | character('z')) >= (character('+') | character('-'));
        auto parse_tool = ParsecT<decltype(parser)>(parser);

        auto res1 = parse_tool(new input_t("x+y-z+x+x+x-y"));
        REQUIRE(res1.status == true);
        auto out1 = res1.actual;
        REQUIRE(out1[0].second == 'x');
        REQUIRE(out1[1].first == '+');
        REQUIRE(out1[1].second == 'y');
        REQUIRE(out1[2].first == '-');
        REQUIRE(out1[2].second == 'z');
        REQUIRE(out1[3].first == '+');
        REQUIRE(out1[3].second == 'x');
        REQUIRE(out1[4].first == '+');
        REQUIRE(out1[4].second == 'x');
        REQUIRE(out1[5].first == '+');
        REQUIRE(out1[5].second == 'x');
        REQUIRE(out1[6].first == '-');
        REQUIRE(out1[6].second == 'y');
    }

    SECTION("Test right recursive combinator.") {
        REQUIRE(true);
    }
}


