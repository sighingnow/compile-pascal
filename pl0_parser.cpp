#include "parsec.hpp"

struct pl0_digit;
struct pl0_char;
struct pl0_write_stmt;

// 数字
struct pl0_digit {
    using parser_type = char;
    constexpr pl0_digit() {} 
    string name() const { return "pl0 digit"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        return digit(text);
    }
} constexpr pl0_digit;

// 字母
struct pl0_alpha {
    using parser_type = char;
    constexpr pl0_alpha() {}
    string name() const { return "pl0 alpha"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        return alpha(text);
    }
} constexpr pl0_alpha;

// 字符
struct pl0_char {
    using parser_type = char;
    constexpr pl0_char() {}
    string name() const { return "pl0 char"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto parser = character('\'') >> (pl0_digit | pl0_alpha) << character('\'');
        return parser(text);
    }
} constexpr pl0_char;

// 字符串
struct pl0_charseq {
    using parser_type = string;
    constexpr pl0_charseq() {}
    string name() const { return "pl0 charseq"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto parser = character('"') >> (++(print - character('"'))) << character('"');
        return parser(text);
    }
} constexpr pl0_charseq;

// 无符号整数
struct pl0_unsigned {
    using parser_type = unsigned int;
    constexpr pl0_unsigned() {}
    string name() const { return "pl0 unsigned"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto parser = pl0_digit++ / ([](vector<char> vec) {
            unsigned int ans = 0; for (auto k: vec) { ans = ans * 10 + (k-'0'); } return ans;
        });
        return parser(text);
    }
} constexpr pl0_unsigned;

// 常量
struct pl0_const {
    using parser_type = int;
    constexpr pl0_const() {}
    string name() const { return "pl0 const"; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto sign = ~character('+');
        auto ascii_n = pl0_char / [](const char & c) { return (int)c; };
        auto res1 = sign(text);
        if (res1.first != -1) {
            int flag = res1.second.first == '+' ? (1) : (-1);
            auto res2 = pl0_unsigned(drop(text, 1));
            if (res2.first == -1) {
                return res2;
            }
            else {
                return make_pair(1 + res2.first, make_pair(res2.second.first * flag, res2.second.second));
            }
        }
        else {
            return (pl0_unsigned | ascii_n)(text);
        }
    }
} constexpr pl0_const;

// 标识符
struct pl0_identify {
    unsigned parser_type = string;
    constexpr pl0_identify() {}
    pair<int, pair<parser_type, parser_type>> operator () (string const & text) const {
        auto parser = pl0_alpha + (++(pl0_alpha | pl0_digit));
        return parser(text);
    }
} constexpr pl0_identify;

