/**
 * Parser for PL0 grammar.
 */

#ifndef __PLO_PARSER_HPP__
#define __PLO_PARSER_HPP__

#include <tuple>
#include "parsec.hpp"
using namespace std;

// <数字> ::= 0|1|2|3 ... 8|9
struct pl0_digit {
    using parser_type = char;
    constexpr pl0_digit() {} 
    string name() const { return "pl0 digit"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        return digit(text);
    }
} constexpr pl0_digit;

// <字母> ::= a|b|c|d ... x|y|z |A|B…|Z
struct pl0_alpha {
    using parser_type = char;
    constexpr pl0_alpha() {}
    string name() const { return "pl0 alpha"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        return alpha(text);
    }
} constexpr pl0_alpha;

// <字符> ::= '<字母>' | '<数字>'
struct pl0_char {
    using parser_type = char;
    constexpr pl0_char() {}
    string name() const { return "pl0 char"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto parser = character('\'') >> (pl0_digit | pl0_alpha) << character('\'');
        return parser(text);
    }
} constexpr pl0_char;

// <字符串> ::= "{十进制编码为32,33,35-126的ASCII字符}"
struct pl0_charseq {
    using parser_type = string;
    constexpr pl0_charseq() {}
    string name() const { return "pl0 charseq"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        function<parser_type (vector<char>)> fn = [](const vector<char> & vec) {
            string ans; for (char c: vec) { ans.push_back(c); } return ans;
        };
        auto parser = character('"') >> (++(print-character('"'))) << character('"');
        return (spaces >> (parser / fn))(text);
    }
} constexpr pl0_charseq;

// <无符号整数> ::= <数字>{<数字>}
struct pl0_unsigned {
    using parser_type = unsigned int;
    constexpr pl0_unsigned() {}
    string name() const { return "pl0 unsigned"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        function<parser_type (vector<char>)> fn = [](vector<char> vec) {
            unsigned int ans = 0; for (auto k: vec) { ans = ans * 10 + (k-'0'); } return ans;
        };
        auto parser = (spaces >> (pl0_digit++) << spaces) / fn;
        return parser(text);
    }
} constexpr pl0_unsigned;

// <常量> ::= [+| ]<无符号整数>|<字符>
struct pl0_const {
    using parser_type = int;
    constexpr pl0_const() {}
    string name() const { return "pl0 const"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto sign = ~(character('+') | character('-'));
        function<parser_type (const char)> fn = [](const char & c) { return (int)c; };
        auto ascii_n = pl0_char / fn;
        auto res1 = sign(text);
        if (res1.first != 0) {
            int flag = res1.second.first[0] == '+' ? (1) : (-1);
            auto res2 = pl0_unsigned(drop(text, 1));
            return make_pair(
                res2.first == -1 ? res2.first : (res1.first + res2.first),
                make_pair(res2.second.first * flag, res2.second.second)
            );
        }
        else {
            return (pl0_unsigned | ascii_n)(text);
        }
    }
} constexpr pl0_const;

// <标识符> ::= <字母>{<字母>|<数字>}
struct pl0_identify {
    using parser_type = string;
    constexpr pl0_identify() {}
    string name() const { return "pl0 identify"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto parser = pl0_alpha + (++(pl0_alpha | pl0_digit));
        function<parser_type (pair<char, vector<char>>)> fn = [](pair<char, vector<char>> const & chs) {
            string ans(1, chs.first); for (char c: chs.second) { ans.push_back(c); } return ans;
        };
        return (spaces >> (parser / fn) << spaces)(text);
    }
} constexpr pl0_identify;

// <基本类型> ::= integer | char
struct pl0_primitive_type {
    using parser_type = string;
    constexpr pl0_primitive_type() {}
    string name() const { return "pl0 primitive type"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto parser = string_literal("integer") | string_literal("char");
        return (spaces >> parser << spaces)(text);
    }
} constexpr pl0_primitive_type;

// <类型> ::= <基本类型>|array'['<无符号整数>']' of <基本类型>
struct pl0_type {
    using parser_type = pair<string, int>; // <type, dimension>
    constexpr pl0_type() {}
    string name() const { return "pl0 type"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        function<parser_type (string)> fn = [](string const & t) { return make_pair(t, -1); };
        function<parser_type (pair<unsigned int, string>)> flip = [](pair<unsigned int, string> arg) { return make_pair(arg.second, arg.first); };
        auto parser = (pl0_primitive_type / fn) // not array
            | ((string_literal("array") >> spaces >> (character('[') >> pl0_unsigned << character(']')) << spaces << string_literal("of")) + pl0_primitive_type) / flip; // array
        return (spaces >> parser << spaces)(text);
    }
} constexpr pl0_type;

// <加法运算符> ::= +|-
struct pl0_addop {
    using parser_type = char;
    constexpr pl0_addop() {}
    string name() const { return "pl0 addop"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        return (spaces >> (character('+') | character('-')) << spaces)(text);
    }
} constexpr pl0_addop;

// <乘法运算符> ::= *|/
struct pl0_multop {
    using parser_type = char;
    constexpr pl0_multop() {}
    string name() const { return "pl0 multop"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        return (spaces >> (character('*') | character('/')) << spaces)(text);
    }
} constexpr pl0_multop;

// <变量说明> ::= <标识符>{, <标识符>} : <类型>
struct pl0_vardesc {
    using parser_type = pair<vector<string>, pl0_type::parser_type>; // <identifies, type>
    constexpr pl0_vardesc() {}
    string name() const { return "pl0 var description"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto parser = (pl0_identify % character(',')) + (character(':') >> pl0_type);
        return (spaces >> parser << spaces)(text);
    }
} constexpr pl0_vardesc;

// <变量说明部分> ::= var <变量说明> ; {<变量说明>;}
struct pl0_vardesc_stmt {
    using parser_type = vector<pl0_vardesc::parser_type>;
    constexpr pl0_vardesc_stmt() {}
    string name() const { return "pl0 var description statement."; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto parser = string_literal("var") >> ((pl0_vardesc << character(';'))++);
        return (spaces >> parser << spaces)(text);
    }
} constexpr pl0_vardesc_stmt;

#endif /* __PLO_PARSER_HPP__ */
