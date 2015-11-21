/**
 * Some parsers for parse characters. (use constexpr for performance improvement)
 */

#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "input_t.hpp"
#include "parse_tool.hpp"

pair<int, char> any_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(c != EOF ? (1) : (-1), c);
}
static const class parser_t<char> any("any character", any_fn);

pair<int, char> blank_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isblank(c) ? (1) : (-1), c);
}
static const class parser_t<char> blank("blank", blank_fn);

pair<int, char> cntrl_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::iscntrl(c) ? (1) : (-1), c);
}
static const class parser_t<char> cntrl("cntrl", cntrl_fn);

pair<int, char> space_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isspace(c) ? (1) : (-1), c);
}
static const class parser_t<char> space("space", space_fn);

pair<int, char> end_f_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(c == EOF ? (1) : (-1), c);
}
static const class parser_t<char> end_f("eof", end_f_fn);

pair<int, char> eol_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(c == '\n' ? (1) : (-1), c);
}
static const class parser_t<char> eol("eol", eol_fn);

pair<int, char> tab_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(c == '\t' ? (1) : (-1), c);
}
static const class parser_t<char> tab("tab", tab_fn);

pair<int, char> digit_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isdigit(c) ? (1) : (-1), c);
}
static const class parser_t<char> digit("digit", digit_fn);

pair<int, char> xdigit_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isxdigit(c) ? (1) : (-1), c);
}
static const class parser_t<char> xdigit("xdigit", xdigit_fn);

pair<int, char> upper_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isupper(c) ? (1) : (-1), c);
}
static const class parser_t<char> upper("upper", upper_fn);

pair<int, char> lower_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::islower(c) ? (1) : (-1), c);
}
static const class parser_t<char> lower("lower", lower_fn);

pair<int, char> alpha_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isalpha(c) ? (1) : (-1), c);
}
static const class parser_t<char> alpha("alpha", alpha_fn);

pair<int, char> alnum_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isalnum(c) ? (1) : (-1), c);
}
static const class parser_t<char> alnum("alnum", alnum_fn);

pair<int, char> print_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isprint(c) ? (1) : (-1), c);
}
static const class parser_t<char> print("print", print_fn);

pair<int, char> graph_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isgraph(c) ? (1) : (-1), c);
}
static const class parser_t<char> graph("graph", graph_fn);


pair<int, char> character_helper(char const & ch, input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(ch == c ? (1) : (-1), ch);
}
parser_t<char> character(char const & ch) {
    auto fn = std::bind(character_helper, ch, std::placeholders::_1);
    return parser_t<char>("character: " + string(1, ch), fn);
}

pair<int, string> string_literal_helper(const string & str, input_t *text) {
    if (text->empty()) { return make_pair(-1, "\0"); }
    string s = text->take(str.length());
    return make_pair(str == s ? str.length() : (-1), str);
}
parser_t<string> string_literal(string const & str) {
    auto fn = std::bind(string_literal_helper, str, std::placeholders::_1);
    return parser_t<string>("string literal: " + str, fn);
}

pair<int, char> one_of_helper(string const & options, input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(options.find(c) != options.npos ? (1) : (-1), c);
}
parser_t<char> one_of(string const & options) {
    auto fn = std::bind(one_of_helper, options, std::placeholders::_1);
    return parser_t<char>("one of " + options, fn);
}

pair<int, char> no_one_of_helper(string const & options, input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(options.find(c) == options.npos ? (1) : (-1), c);
}
parser_t<char> no_one_of(string const & options) {
    auto fn = std::bind(no_one_of_helper, options, std::placeholders::_1);
    return parser_t<char>("one of " + options, fn);
}


#endif /* __PARSER_HPP__ */
