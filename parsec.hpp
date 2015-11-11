/********************************************************
 * libparsec
 *
 * Description:     A univeral C++ parser combinator library 
 *                  inspirted by Parsec library of Haskell.
 * Author:          He Tao
 *
 *********************************************************/

#include <iostream>
#include <string>
#include <functional>
#include <exception>
#include <vector>
using namespace std;


/**
 * Some parsers for parse characters. (use constexpr for performance improvement)
 */
struct any {
    using is_parser_type = true_type;
    constexpr any() {}
    string name() const { return "any character"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(c != EOF ? (1) : (-1), make_pair(c, c));
    }
} constexpr any;

struct blank {
    using is_parser_type = true_type;
    constexpr blank() {}
    string name() const { return "blank"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isblank(c) ? (1) : (-1), make_pair(c, ' '));
    }
} constexpr blank;

struct cntrl {
    using is_parser_type = true_type;
    constexpr cntrl() {}
    string name() const { return "cntrl"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::iscntrl(c) ? (1) : (-1), make_pair(c, '\x94'));
    }
} constexpr cntrl;

struct space {
    using is_parser_type = true_type;
    constexpr space() {}
    string name() const { return "space"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isspace(c) ? (1) : (-1), make_pair(c, ' '));
    }
} constexpr space;

struct eof {
    using is_parser_type = true_type;
    constexpr eof() {}
    string name() const { return "eof"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(c == EOF ? (1) : (-1), make_pair(c, EOF));
    }
} constexpr eof;

struct eol {
    using is_parser_type = true_type;
    constexpr eol() {}
    string name() const { return "eol"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(c == '\n' ? (1) : (-1), make_pair(c, '\n'));
    }
} constexpr eol;

struct tab {
    using is_parser_type = true_type;
    constexpr tab() {}
    string name() const { return "tab"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(c == '\t' ? (1) : (-1), make_pair(c, '\t'));
    }
} constexpr tab;

struct punct {
    using is_parser_type = true_type;
    constexpr punct() {}
    string name() const { return "punctuation"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isspace(c) ? (1) : (-1), make_pair(c, '.'));
    }
} constexpr punct;

struct digit {
    using is_parser_type = true_type;
    constexpr digit() {}
    string name() const { return "digit"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isdigit(c) ? (1) : (-1), make_pair(c, '0'));
    }
} constexpr digit;

struct xdigit {
    using is_parser_type = true_type;
    constexpr xdigit() {}
    string name() const { return "xdigit"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isxdigit(c) ? (1) : (-1), make_pair(c, 'x'));
    }
} constexpr xdigit;

struct upper {
    using is_parser_type = true_type;
    constexpr upper() {}
    string name() const { return "upper"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isupper(c) ? (1) : (-1), make_pair(c, 'u'));
    }
} constexpr upper;

struct lower {
    using is_parser_type = true_type;
    constexpr lower() {}
    string name() const { return "lower"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::islower(c) ? (1) : (-1), make_pair(c, 'l'));
    }
} constexpr lower;

struct alpha {
    using is_parser_type = true_type;
    constexpr alpha() {}
    string name() const { return "alpha"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isalpha(c) ? (1) : (-1), make_pair(c, 'a'));
    }
} constexpr alpha;

struct alnum {
    using is_parser_type = true_type;
    constexpr alnum() {}
    string name() const { return "alnum"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isalnum(c) ? (1) : (-1), make_pair(c, 'n'));
    }
} constexpr alnum;

struct print {
    using is_parser_type = true_type;
    constexpr print() {}
    string name() const { return "print"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isprint(c) ? (1) : (-1), make_pair(c, 'p'));
    }
} constexpr print;

struct graph {
    using is_parser_type = true_type;
    constexpr graph() {}
    string name() const { return "graph"; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(::isgraph(c) ? (1) : (-1), make_pair(c, 'g'));
    }
} constexpr graph;

class one_of {
    string _options;
public:
    using is_parser_type = true_type;
    one_of() {}
    string name() const { return "one of " + _options; }
    one_of operator () (string const & options) { _options = options; return *this; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(_options.find(c) != _options.npos ? (1) : (-1), make_pair(c, c));
    }
} one_of;

class no_one_of {
    string _options;
public:
    using is_parser_type = true_type;
    no_one_of() {}
    string name() const { return "no one of " + _options; }
    no_one_of operator () (string const & options) { _options = options; return *this; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(_options.find(c) == _options.npos ? (1) : (-1), make_pair(c, c));
    }
} no_one_of;

class character {
    char _c;
public:
    using is_parser_type = true_type;
    character() {}
    string name() const { return "character '" + string(1, _c) + "'"; }
    character operator () (char const & c) { _c = c; return *this; }
    pair<int, pair<char, char>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.at(0);
        return make_pair(_c == c ? (1) : (-1), make_pair(c, _c));
    }
} character;

class string_literal {
    string _s;
public:
    using is_parser_type = true_type;
    string_literal() {}
    string name() const { return "string \"" + _s + "\""; }
    string_literal operator () (string const & s) { _s = s; return *this; }
    pair<int, pair<string, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<string, string>()); }
        string s = text.substr(0, _s.length());
        return make_pair(_s == s ? s.length() : (-1), make_pair(s, _s));
    }
} string_literal;

/**
 * Parser combinators.
 */

template<typename PA, typename PB, typename V>
class compose {
private:
    PA const pa;
    PB const pb;
public:
    using is_parser_type = true_type;
    constexpr compose(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "compose two parser."; }
    pair<int, pair<V, V>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first == -1) {
            return res1;
        }
        return pb(text.substr(res1.first));
    }
};
template<typename PA, typename PB, typename V>
constexpr compose<PA, PB, V> const operator >> (PA const & pa, PB const & pb) {
    return compose<PA, PB, V>(pa, pb);
}

template<typename PA, typename PB, typename V>
class choice {
private:
    PA const pa;
    PB const pb;
public:
    using is_parser_type = true_type;
    constexpr choice(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "choice from two parser."; }
    pair<int, pair<V, V>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first != -1) {
            return res1;
        }
        return pb(text);
    }
};
template<typename PA, typename PB, typename V>
constexpr choice<PA, PB, V> const operator | (PA const & pa, PB const & pb) {
    return choice<PA, PB, V>(pa, pb);
}

template<typename PA, typename PB, typename V>
class try_choice {
private:
    PA const pa;
    PB const pb;
public:
    using is_parser_type = true_type;
    constexpr try_choice(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "try choice from two parser with backtracking."; }
    pair<int, pair<V, V>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first != -1) {
            return res1;
        }
        return pb(text);
    }
};
template<typename PA, typename PB, typename V>
constexpr try_choice<PA, PB, V> const operator ^ (PA const & pa, PB const & pb) {
    return try_choice<PA, PB, V>(pa, pb);
}

template<typename PA, typename PB, typename V>
class joint {
private:
    PA const pa;
    PB const pb;
public:
    using is_parser_type = true_type;
    constexpr joint(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "joint two parser."; }
    pair<int, pair<vector<V>, vector<V>>> operator () (string const & text) const {
        vector<V> actual, expected;
        auto res1 = pa(text);
        actual.push_back(res1.second.first); expected.push_back(res1.second.second);
        if (res1.first == -1) {
            return make_pair(-1, make_pair(actual, expected));
        }
        auto res2 = pb(text.substr(res1.first));
        actual.push_back(res2.second.first); expected.push_back(res2.second.second);
        if (res2.first == -1) {
            return make_pair(-1, make_pair(actual, expected));
        }
        else {
            return make_pair(res1.first + res2.first, actual, expected);
        }
    }
};
template<typename PA, typename PB, typename V>
constexpr joint<PA, PB, V> const operator + (PA const & pa, PB const & pb) {
    return joint<PA, PB, V>(pa, pb);
}

template<typename PA, typename PB, typename V>
class skip {
private:
    PA const pa;
    PB const pb;
public:
    using is_parser_type = true_type;
    constexpr skip(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "skip the second parser."; }
    pair<int, pair<V, V>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first == -1) {
            return res1;
        }
        auto res2 = pb(text.substr(res1.first));
        if (res2.first == -1) {
            return res2;
        }
        // the `skip` combinator DOES consume the end flag. see also `ends_with` combinator.
        return make_pair(res1.first + res2.first, res1.second);
    }
};
template<typename PA, typename PB, typename V>
constexpr skip<PA, PB, V> const operator << (PA const & pa, PB const & pb) {
    return skip<PA, PB, V>(pa, pb);
}

template<typename PA, typename PB, typename V>
class ends_with {
private:
    PA const pa;
    PB const pb;
public:
    using is_parser_type = true_type;
    constexpr ends_with(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "ends with the second parser."; }
    pair<int, pair<V, V>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first == -1) {
            return res1;
        }
        auto res2 = pb(text.substr(res1.first));
        if (res2.first == -1) {
            return res2;
        }
        // the `ends_with` combinator DOES NOT consume the end flag. see also `skip` combinator.
        return res1;
    }
};
template<typename PA, typename PB, typename V>
constexpr ends_with<PA, PB, V> const operator < (PA const & pa, PB const & pb) {
    return ends_with<PA, PB, V>(pa, pb);
}

template<typename PA, typename V>
class times {
private:
    PA const pa;
    int const mint, maxt;
public:
    using is_parser_type = true_type;
    constexpr times(PA const & pa, int const & mint, int const & maxt): pa(pa), mint(mint), maxt(maxt) {}
    string name() const { return "repeat a parser many times."; }
    virtual pair<int, pair<vector<V>, vector<V>>> operator () (string const & text) const {
        int cnt = 0, offset = 0;
        vector<V> actual, expected;
        string s = text;
        while (cnt < maxt) {
            auto res = pa(text);
            if (res.first == -1) {
                if (cnt < mint) {
                    offset = -1; // failed.
                }
                break;
            }
            cnt = cnt + 1; // update counter.
            offset = offset + res.first; // update length.
            actual.push_back(res.second.first);
            expected.push_back(res.second.second);
            if (cnt >= maxt) {
                break;
            }
            if ((int)s.length() > res.first) {
                s = s.substr(res.first); // update text stream.
            }
            else {
                if (cnt < mint) {
                    offset = -1; // failed.
                }
                break;
            }
        }
        return make_pair(offset, make_pair(actual, expected));
    }
};

template<typename PA, typename V>
struct many: public times<PA, V> {
public:
    using is_parser_type = true_type;
    constexpr many(PA const & pa): times<PA, V>(pa, 0, 0x7fffffff) {}
    string name() const { return "many: repeat 0 or more times."; }
};

template<typename PA, typename V>
struct many1: public times<PA, V> {
public:
    using is_parser_type = true_type;
    constexpr many1(PA const & pa): times<PA, V>(pa, 1, 0x7fffffff) {}
    string name() const { return "many1: repeat 1 or more times."; }
};

template<typename PA, typename V>
struct countk: public times<PA, V> {
public:
    using is_parser_type = true_type;
    constexpr countk(PA const & pa, const int & k): times<PA, V>(pa, k, k) {}
    string name() const { return "countk: repeat exactly k times."; }
};
template<typename PA, typename V>
constexpr countk<PA, V> const operator * (PA const & pa, int const & k) {
    return countk<PA, V>(pa, k);
}

/**
 * The result representation of a parser. `Success` and `Failure` are used to construct Value object, while
 * `Aggregate` is used to accmulate two Value object into one.
 */
template<typename V>
struct ValueT {
    bool const status;
    pair<int, int> loc;
    int len;
    V const actual, expected;

    string name() const {
        return "stream @ " + ::to_string(loc.first) + ":" + ::to_string(loc.second)
            + ", " + (status ? "true" : "false") + ", length: " + ::to_string(len) + "\n"
            + "    actual:   " + "vector" + "\n"
            + "    expected: " + "vector" + "\n";
    }

    ValueT<V>(bool status, pair<int, int> loc, int len, V const actual, V const expected)
        : status(status), loc(loc), len(len), actual(actual), expected(expected) {}

    static ValueT<V> Success(pair<int, int> loc, int len, V const actual, V const expected) {
        return ValueT<V>(true, loc, len, actual, expected);
    }

    static ValueT<V> Failure(pair<int, int> loc, int len, V const actual, V const expected) {
        return ValueT<V>(false, loc, len, actual, expected);
    }

    // static ValueT<V> Aggregate(ValueT<V> const a, ValueT<V> const b) {
    //     if (!a.status) return a;
    //     if (!b.status) return b;
    //     return ValueT<V>::Success(b.sname, b.loc, a.len + b.len, a.actual.push_back(b.actual), a.expected.push_back(b.expected));
    // }
};

// template<typename V>
struct ParseError: public exception {
    string const sname, message;
    pair<int, int> const loc;
    // V const actual, expected;
    ParseError(string const & sname, string const & message, pair<int, int> const loc): exception(), sname(sname), message(message), loc(loc) {}
    const char *what() const noexcept { return (sname + ": " + message).c_str(); }
};

/**
 * Parser model, wraps a function parse a Value object from a stream at specified position.
 */
template<typename V, typename P, typename = typename P::is_parser_type>
struct ParsecT {
private:
    P const parser;
public:
    constexpr ParsecT<P, V>(P const & parser): parser(parser) {}
    string name() const { return "ParsecT"; }
    ValueT<V> parse(string const & text) const {
        pair<int, int> loc = make_pair(1, 1);
        pair<int, pair<V, V>> res;
        try { // try exception throwed during operating the stream object.
            res = parser(text);
        } catch (std::exception & e) {
            cout << "!!!Exception: " + string(e.what()) << endl; 
            res = ::pair<int, pair<V, V>>(-1, pair<V, V>());
        }
        // text = text.substr(res.first); // move ahead offset.
        return ValueT<V>(res.first != -1, loc, res.first, res.second.first, res.second.second);
    }
    ValueT<V> operator () (string const & text) const {
        return this->parse(text);
    } 
};


