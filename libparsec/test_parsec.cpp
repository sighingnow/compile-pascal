#include "gtest/gtest.h"

#include "input_t.hpp"
#include "parser.hpp"
#include "combinator.hpp"
#include "parse_tool.hpp"
#include "patch.hpp"

struct spaces {
    using parser_type = string;
    constexpr spaces() {}
    string name() const { return "spaces"; }
    tuple<int, parser_type, string> operator () (input_t *text) const {
        function<string (vector<char>)> fn = [](vector<char> const & vec) {
            string ans; for (auto c: vec) { ans.push_back(c); } return ans;
        };
        auto parser = (++space) / fn;
        return parser(text);
    }
} static const spaces;

// Primitive parsers.

TEST(Primitive, AnyCharacter) {
    auto parser = any;
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    input_t *s1 = new input_t("abcdeABCDE12345");
    auto res1 = parse_tool(s1);
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual, 'a');
}

TEST(Primitive, BlankCharacter) {
    auto parser = blank;
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    input_t *s1 = new input_t(" x");
    auto res1 = parse_tool(s1);
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual, ' ');
}

TEST(Primitive, InvalidBlankCharacter) {
    auto parser = blank;
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    input_t *s1 = new input_t("x");
    auto res1 = parse_tool(s1);
    EXPECT_EQ(res1.status, false);   
}

TEST(Primitive, SingleSpecifiedCharacter) {
    auto parser = character('a');
    auto parse_tool = ParsecT<decltype(parser)>(parser);
    
    input_t *s1 = new input_t("abcde");
    auto res1 = parse_tool(s1);
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual, 'a');

    input_t *s2 = new input_t("x");
    auto res2 = parse_tool(s2);
    EXPECT_EQ(res2.status, false);
}

TEST(Primitive, StringLiteral) {
    auto parser = string_literal("abcde");
    auto parse_tool = ParsecT<decltype(parser)>(parser);
    
    input_t *s1 = new input_t("abcdeabcde");
    auto res1 = parse_tool(s1);
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual, "abcde");

    input_t *s2 = new input_t("x");
    auto res2 = parse_tool(s2);
    EXPECT_EQ(res2.status, false);
}

// Parser combinators.

TEST(ParserCombinators, Compose) {
    auto parser = character('x') >> character('y');
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("xy"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual, 'y');

    auto res2 = parse_tool(new input_t("x"));
    EXPECT_EQ(res2.status, false);

    auto res3 = parse_tool(new input_t("y"));
    EXPECT_EQ(res3.status, false);
}

TEST(ParserCombinators, Choice) {
    auto parser = character('x') | character('y');
    auto parse_tool = ParsecT<decltype(parser)>(parser);
    
    auto res1 = parse_tool(new input_t("xy"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual, 'x');

    auto res2 = parse_tool(new input_t("yx"));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual, 'y');

    auto res3 = parse_tool(new input_t("zzz"));
    EXPECT_EQ(res3.status, false);
}

TEST(ParserCombinators, Joint) {
    auto parser = character('x') + character('y');
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("xy"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.len, 2);
    EXPECT_EQ(res1.actual.first, 'x');
    EXPECT_EQ(res1.actual.second, 'y');
}

TEST(ParserCombinators, Skip) {
    auto parser = character('x') << character('y');
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("xy"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.len, 2);
    EXPECT_EQ(res1.actual, 'x');

    auto res2 = parse_tool(new input_t("xz"));
    EXPECT_EQ(res2.status, false);
}

TEST(ParserCombinators, SkipAndJoint) {
    auto parser = (character('x') << character('y')) + (character('a') << character('b'));
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("xyab"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual.first, 'x');
    EXPECT_EQ(res1.actual.second, 'a');
    EXPECT_EQ(res1.len, 4);

    auto res2 = parse_tool(new input_t("xzab"));
    EXPECT_EQ(res2.status, false);

    auto res3 = parse_tool(new input_t("xyac"));
    EXPECT_EQ(res3.status, false);
}

TEST(ParserCombinators, EndsWith) {
    auto parser = (spaces >> string_literal("abcd") < character('a')) >> string_literal("abcd") < character(',');
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res = parse_tool(new input_t("abcdabcd,"));
    EXPECT_EQ(res.status, true);
    EXPECT_EQ(res.actual, "abcd");
    EXPECT_EQ(res.len, 8);
}

TEST(ParserCombinators, Except) {
    auto parser = (character('x') | character('y') | character('z')) - character('y');
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("x"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual, 'x');

    auto res2 = parse_tool(new input_t("y"));
    EXPECT_EQ(res2.status, false);

    auto res3 = parse_tool(new input_t("z"));
    EXPECT_EQ(res3.status, true);
    EXPECT_EQ(res3.actual, 'z');
}

// Repeat combinators.

function<string (vector<char>)> fn = [](vector<char> const & vec) {
    string ans; for (auto c: vec) { ans.push_back(c); } return ans;
};

TEST(RepeatCombinators, Times) {
    auto parser = times<decltype(character('x'))>(character('x'), 2, 10) / fn;
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("xxx"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual, "xxx");

    auto res2 = parse_tool(new input_t("xxxxxxxxxxxxxxxx"));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual, "xxxxxxxxxx");

    auto res3 = parse_tool(new input_t("xxxxk"));
    EXPECT_EQ(res3.status, true);
    EXPECT_EQ(res3.actual, "xxxx");
    EXPECT_EQ(res3.len, 4);

    auto res4 = parse_tool(new input_t("x"));
    EXPECT_EQ(res4.status, false);
}

TEST(RepeatCombinators, Optional) {
    auto parser = (~(++digit)) + (~string_literal("abcde"));
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("abcdec"));
    auto out1 = res1.actual;
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(out1.first.size(), (size_t)0);
    EXPECT_EQ(out1.second, "abcde");

    auto res2 = parse_tool(new input_t("525435d"));
    auto out2 = res2.actual;
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(out2.first[2], '5');
    EXPECT_EQ(out2.second, "");
}

TEST(RepeatCombinators, RepeatZeroTimes) {
    auto parser = times<decltype(character('x'))>(character('x'), 0, 0) / fn;
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("xxx"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.len, 0);

    auto res2 = parse_tool(new input_t(""));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual, "");
}

TEST(RepeatCombinators, Many) {
    auto parser = ++any / fn;
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    EXPECT_EQ(parse_tool(new input_t("x")).actual, "x");
    EXPECT_EQ(parse_tool(new input_t("x y z")).actual, "x y z");
    EXPECT_EQ(parse_tool(new input_t("")).status, true);
}

TEST(RepeatCombinators, Spaces) {
    auto parser = spaces;
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    input_t *s1 = new input_t("          ");
    EXPECT_EQ(parse_tool(s1).len, (int)s1->length());

    input_t *s2 = new input_t("     abcd");
    EXPECT_EQ(parse_tool(s2).len, 5);
}

TEST(RepeatCombinators, Many1) {
    auto parser = any++ / fn;
    auto parse_tool = ParsecT<decltype(parser)>(parser);
    
    EXPECT_EQ(parse_tool(new input_t("x")).actual, "x");
    EXPECT_EQ(parse_tool(new input_t("x y z")).actual, "x y z");
    EXPECT_EQ(parse_tool(new input_t("")).status, false);
}

TEST(RepeatCombinators, Count) {
    auto parser = any * 5 / fn;
    auto parse_tool = ParsecT<decltype(parser)>(parser);
    
    EXPECT_EQ(parse_tool(new input_t("x")).status, false);
    EXPECT_EQ(parse_tool(new input_t("x y z   z")).actual, "x y z");
    EXPECT_EQ(parse_tool(new input_t("123456")).actual, "12345");
}

TEST(RepeatCombinators, Sepby1) {
    auto parser = (character('x') % character('y')) / fn;
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("xyxyxyx"));
    EXPECT_EQ(res1.status, true);
    EXPECT_EQ(res1.actual, "xxxx");
    EXPECT_EQ(res1.len, 7);

    auto res2 = parse_tool(new input_t("xyxyxyxy"));
    EXPECT_EQ(res2.status, true);
    EXPECT_EQ(res2.actual, "xxxx");
    EXPECT_EQ(res2.len, 7);
}

// Chain combinator for recursive grammars.

TEST(ChainCombinators, LeftRecursive) {
    auto parser = (character('x') | character('y') | character('z')) >= (character('+') | character('-'));
    auto parse_tool = ParsecT<decltype(parser)>(parser);

    auto res1 = parse_tool(new input_t("x+y-z+x+x+x-y"));
    EXPECT_EQ(res1.status, true);
    auto out1 = res1.actual;
    EXPECT_EQ(out1[0].second, 'x');
    EXPECT_EQ(out1[1].first, '+');
    EXPECT_EQ(out1[1].second, 'y');
    EXPECT_EQ(out1[2].first, '-');
    EXPECT_EQ(out1[2].second, 'z');
    EXPECT_EQ(out1[3].first, '+');
    EXPECT_EQ(out1[3].second, 'x');
    EXPECT_EQ(out1[4].first, '+');
    EXPECT_EQ(out1[4].second, 'x');
    EXPECT_EQ(out1[5].first, '+');
    EXPECT_EQ(out1[5].second, 'x');
    EXPECT_EQ(out1[6].first, '-');
    EXPECT_EQ(out1[6].second, 'y');
}

TEST(ChainCombinators, RightRecursive) {
    EXPECT_TRUE(true);
}


