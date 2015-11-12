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

string drop(string const & text, int const & k) {
    if ((int)text.length() < k) {
        return "";
    }
    else {
        return text.substr(k);
    }
}

/**
 * Some parsers for parse characters. (use constexpr for performance improvement)
 */
struct any {
    using parser_type = char;
    constexpr any() {}
    string name() const { return "any character"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(c != EOF ? (1) : (-1), make_pair(c, name()));
    }
} constexpr any;

struct blank {
    using parser_type = char;
    constexpr blank() {}
    string name() const { return "blank"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isblank(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr blank;

struct cntrl {
    using parser_type = char;
    constexpr cntrl() {}
    string name() const { return "cntrl"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::iscntrl(c) ? (1) : (-1), make_pair(c, name() + "\\x94"));
    }
} constexpr cntrl;

struct space {
    using parser_type = char;
    constexpr space() {}
    string name() const { return "space"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isspace(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr space;

struct eof {
    using parser_type = char;
    constexpr eof() {}
    string name() const { return "eof"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(c == EOF ? (1) : (-1), make_pair(c, name()));
    }
} constexpr eof;

struct eol {
    using parser_type = char;
    constexpr eol() {}
    string name() const { return "eol"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(c == '\n' ? (1) : (-1), make_pair(c, name() + "\\n"));
    }
} constexpr eol;

struct tab {
    using parser_type = char;
    constexpr tab() {}
    string name() const { return "tab"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(c == '\t' ? (1) : (-1), make_pair(c, name() + "\\t"));
    }
} constexpr tab;

struct punct {
    using parser_type = char;
    constexpr punct() {}
    string name() const { return "punctuation"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isspace(c) ? (1) : (-1), make_pair(c, name() + "."));
    }
} constexpr punct;

struct digit {
    using parser_type = char;
    constexpr digit() {}
    string name() const { return "digit"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isdigit(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr digit;

struct xdigit {
    using parser_type = char;
    constexpr xdigit() {}
    string name() const { return "xdigit"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isxdigit(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr xdigit;

struct upper {
    using parser_type = char;
    constexpr upper() {}
    string name() const { return "upper"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isupper(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr upper;

struct lower {
    using parser_type = char;
    constexpr lower() {}
    string name() const { return "lower"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::islower(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr lower;

struct alpha {
    using parser_type = char;
    constexpr alpha() {}
    string name() const { return "alpha"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isalpha(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr alpha;

struct alnum {
    using parser_type = char;
    constexpr alnum() {}
    string name() const { return "alnum"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isalnum(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr alnum;

struct print {
    using parser_type = char;
    constexpr print() {}
    string name() const { return "print"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isprint(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr print;

struct graph {
    using parser_type = char;
    constexpr graph() {}
    string name() const { return "graph character"; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(::isgraph(c) ? (1) : (-1), make_pair(c, name()));
    }
} constexpr graph;

class one_of {
    string _options;
public:
    using parser_type = char;
    one_of() {}
    string name() const { return "one of " + _options; }
    one_of operator () (string const & options) { _options = options; return *this; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(_options.find(c) != _options.npos ? (1) : (-1), make_pair(c, name()));
    }
} one_of;

class no_one_of {
    string _options;
public:
    using parser_type = char;
    no_one_of() {}
    string name() const { return "no one of " + _options; }
    no_one_of operator () (string const & options) { _options = options; return *this; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(_options.find(c) == _options.npos ? (1) : (-1), make_pair(c, name()));
    }
} no_one_of;

class character {
    char _c;
public:
    using parser_type = char;
    character() {}
    string name() const { return "character '" + string(1, _c) + "'"; }
    character operator () (char const & c) { _c = c; return *this; }
    pair<int, pair<char, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<char, string>()); }
        char c = text.at(0);
        return make_pair(_c == c ? (1) : (-1), make_pair(c, name()));
    }
} character;

class string_literal {
    string _s;
public:
    using parser_type = string;
    string_literal() {}
    string name() const { return "string \"" + _s + "\""; }
    string_literal operator () (string const & s) { _s = s; return *this; }
    pair<int, pair<string, string>> operator () (string const & text) const {
        if (text.empty()) { return make_pair(-1, ::pair<string, string>()); }
        string s = text.substr(0, _s.length());
        return make_pair(_s == s ? s.length() : (-1), make_pair(s, name()));
    }
} string_literal;

/**
 * Parser combinators.
 */

template<typename PA, typename PB>
class compose {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PB::parser_type;
    constexpr compose(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "compose two parser."; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first == -1) {
            return res1;
        }
        auto res2 = pb(drop(text, res1.first));
        if (res2.first == -1) {
            return res2;
        }
        return make_pair(res1.first + res2.first, res2.second);
    }
};
// operator '>>'
template<typename PA, typename PB>
constexpr compose<PA, PB> const operator >> (PA const & pa, PB const & pb) {
    return compose<PA, PB>(pa, pb);
}

template<typename PA, typename PB>
class choice {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr choice(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "choice from two parser."; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first != -1) {
            return res1;
        }
        return pb(text);
    }
};
// operator '|'
template<typename PA, typename PB>
constexpr choice<PA, PB> const operator | (PA const & pa, PB const & pb) {
    return choice<PA, PB>(pa, pb);
}

template<typename PA, typename PB>
class try_choice {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr try_choice(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "try choice from two parser with backtracking."; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first != -1) {
            return res1;
        }
        return pb(text);
    }
};
// operator '^'
template<typename PA, typename PB>
constexpr try_choice<PA, PB> const operator ^ (PA const & pa, PB const & pb) {
    return try_choice<PA, PB>(pa, pb);
}

template<typename PA, typename PB>
class joint {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr joint(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "joint two parser."; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        parser_type actual;
        string expected;
        auto res1 = pa(text);
        actual.push_back(res1.second.first);
        expected += res1.second.second + ", ";
        if (res1.first == -1) {
            return make_pair(-1, make_pair(actual, expected));
        }
        auto res2 = pb(drop(text, res1.first));
        actual.push_back(res2.second.first);
        expected += res2.second.second + ", ";
        if (res2.first == -1) {
            return make_pair(-1, make_pair(actual, expected));
        }
        else {
            return make_pair(res1.first + res2.first, make_pair(actual, expected));
        }
    }
};
// operator '+'
template<typename PA, typename PB>
constexpr joint<PA, PB> const operator + (PA const & pa, PB const & pb) {
    return joint<PA, PB>(pa, pb);
}

template<typename PA, typename PB>
class except {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr except(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "except one from two parsers."; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto res1 = pa(text);
        auto res2 = pb(text);
        if (res1.first != -1 && res2.first == -1) {
            return res1;
        }
        else {
            return make_pair(res1.first, make_pair(res1.second.first, "except " + res2.second.second + " from " + res1.second.second));
        }
    }
};
// operator '-'
template<typename PA, typename PB>
constexpr except<PA, PB> const operator - (PA const & pa, PB const & pb) {
    return except<PA, PB>(pa, pb);
}

template<typename PA, typename PB>
class skip {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr skip(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "skip the second parser."; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first == -1) {
            return res1;
        }
        auto res2 = pb(drop(text, res1.first));
        if (res2.first == -1) {
            return res2;
        }
        // the `skip` combinator DOES consume the end flag. see also `ends_with` combinator.
        return make_pair(res1.first + res2.first, res1.second);
    }
};
// operator '<<'
template<typename PA, typename PB>
constexpr skip<PA, PB> const operator << (PA const & pa, PB const & pb) {
    return skip<PA, PB>(pa, pb);
}

template<typename PA, typename PB>
class ends_with {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr ends_with(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "ends with the second parser."; }
    pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto res1 = pa(text);
        if (res1.first == -1) {
            return res1;
        }
        auto res2 = pb(drop(text, res1.first));
        if (res2.first == -1) {
            return res2;
        }
        // the `ends_with` combinator DOES NOT consume the end flag. see also `skip` combinator.
        return res1;
    }
};
// operator '<'
template<typename PA, typename PB>
constexpr ends_with<PA, PB> const operator < (PA const & pa, PB const & pb) {
    return ends_with<PA, PB>(pa, pb);
}

template<typename PA>
class times {
private:
    PA const pa;
    int const mint, maxt;
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr times(PA const & pa, int const & mint, int const & maxt): pa(pa), mint(mint), maxt(maxt) {}
    string name() const { return "repeat a parser many times."; }
    virtual pair<int, pair<parser_type, string>> operator () (string const & text) const {
        int cnt = 0, offset = 0;
        parser_type actual;
        string expected;
        string s = text;
        while (cnt < maxt) {
            auto res = pa(s);
            if (res.first == -1) {
                if (cnt < mint) {
                    offset = -1; // failed.
                }
                break;
            }
            cnt = cnt + 1; // update counter.
            offset = offset + res.first; // update length.
            actual.push_back(res.second.first);
            expected += res.second.second + ", ";
            s = drop(s, res.first); // update text stream.
            if (cnt >= maxt) {
                break;
            }
        }
        return make_pair(offset, make_pair(actual, expected));
    }
};

template<typename PA>
struct optional: public times<PA> {
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr optional(PA const & pa): times<PA>(pa, 0, 1) {}
    string name() const { return "optional: repeat 0 or 1 times."; }
};

template<typename PA>
struct many: public times<PA> {
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr many(PA const & pa): times<PA>(pa, 0, 0x7fffffff) {}
    string name() const { return "many: repeat 0 or more times."; }
};
// operator '++()'
template<typename PA>
constexpr many<PA> const operator ++ (PA const & pa) {
    return many<PA>(pa);
}

template<typename PA>
struct many1: public times<PA> {
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr many1(PA const & pa): times<PA>(pa, 1, 0x7fffffff) {}
    string name() const { return "many1: repeat 1 or more times."; }
};
// operator '()++'
template<typename PA>
constexpr many1<PA> const operator ++ (PA const & pa, int) {
    return many1<PA>(pa);
}

template<typename PA>
struct countk: public times<PA> {
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr countk(PA const & pa, const int & k): times<PA>(pa, k, k) {}
    string name() const { return "countk: repeat exactly k times."; }
};
// operator '*'
template<typename PA>
constexpr countk<PA> const operator * (PA const & pa, int const & k) {
    return countk<PA>(pa, k);
}

template<typename PA, typename T>
class mapfn {
public:
    using parser_type = T;
    using origin_type = typename PA::parser_type;
private:
    PA const pa;
    function<parser_type (origin_type)> const fn;
public:
    constexpr mapfn(PA const & pa, function<parser_type (origin_type)> const & fn): pa(pa), fn(fn) {}
    string name() const { return "apply a function to the result"; }
    virtual pair<int, pair<parser_type, string>> operator () (string const & text) const {
        auto res = pa(text);
        return make_pair(res.first, make_pair(fn(res.second.first), "map fn on " + res.second.second));
    }
};
// operator '/'
template<typename PA, typename T>
constexpr mapfn<PA, T> const operator / (PA const & pa, function<T(typename PA::parser_type)> const & fn) {
    return mapfn<PA, T>(pa, fn);
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
    V const actual;
    string expected;

    string to_string(int n) const { return ::to_string(n); }
    string to_string(char c) const { return string(1, c); }
    string to_string(string s) const { return s; }
    string to_string(vector<char> vec) const { string ans = "vector char: "; for (char c: vec) { ans.push_back(c); ans.push_back(','); } return ans; }
    string to_string(vector<string> vec) const { string ans = "vector string: "; for (string s: vec) { ans += s + ","; } return ans; }

    string name() const {
        return "stream @ " + ::to_string(loc.first) + ":" + ::to_string(loc.second)
            + ", " + (status ? "true" : "false") + ", length: " + ::to_string(len) + "\n"
            + "    actual:   " + to_string(actual) + "\n"
            + "    expected: " + to_string(expected) + "\n";
    }

    ValueT<V>(bool status, pair<int, int> loc, int len, V const & actual, string const & expected)
        : status(status), loc(loc), len(len), actual(actual), expected(expected) {}

    static ValueT<V> Success(pair<int, int> loc, int len, V const & actual, string const & expected) {
        return ValueT<V>(true, loc, len, actual, expected);
    }

    static ValueT<V> Failure(pair<int, int> loc, int len, V const & actual, string const & expected) {
        return ValueT<V>(false, loc, len, actual, expected);
    }
};

// template<typename V>
struct ParseError: public exception {
    string const sname, message;
    pair<int, int> const loc;
    ParseError(string const & sname, string const & message, pair<int, int> const loc): exception(), sname(sname), message(message), loc(loc) {}
    const char *what() const noexcept { return (sname + ": " + message).c_str(); }
};

/**
 * Parser model, wraps a function parse a Value object from a stream at specified position.
 */
template<typename P>
struct ParsecT {
private:
    P const parser;
public:
    using V = typename P::parser_type;
    constexpr ParsecT<P>(P parser): parser(parser) {}
    string name() const { return "ParsecT"; }
    ValueT<V> parse(string const & text) const {
        pair<int, int> loc = make_pair(1, 1);
        pair<int, pair<V, string>> res;
        try { // try exception throwed during operating the stream object.
            res = parser(text);
        } catch (std::exception & e) {
            cout << "!!!Exception: " + string(e.what()) << endl; 
            res = ::pair<int, pair<V, string>>(-1, pair<V, string>());
        }
        // text = text.substr(res.first); // move ahead offset.
        return ValueT<V>(res.first != -1, loc, res.first, res.second.first, res.second.second);
    }
    ValueT<V> operator () (string const & text) const {
        return this->parse(text);
    } 
};


