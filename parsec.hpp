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
using namespace std;

/**
 * The abstract representation of source stream. A specified stream(eg. string is a representation
 * of char stream) can be constructed by template specialization technique. A stream should have 
 * these methods:
 *
 *  + head(): return the first element in the stream.
 *  + at(int idx): return the idx'th element in the stream.
 *  + tail(): return the stream except for the first element.
 *  + take(int k): return the stream contains the first k elements.
 *  + drop(int k): return the stream not contains the first k elements.
 */
template<typename S, typename T>
struct StreamT {
    string const name;
    S const source;
    int row, column, count;
    StreamT<S, T>(string const name, S source): name(name), source(source) {}
    virtual T head() const = 0;
    virtual void next() const = 0;
    virtual void next(int const k) const = 0;
    virtual pair<int, int> locate() const = 0;
    virtual StreamT<S, T> tail() const = 0;
    virtual StreamT<S, T> take(int k) const = 0;
    virtual StreamT<S, T> drop(int k) const = 0;
    virtual S extract() const = 0;
};

/**
 * Using template specialization to construct a char stream.
 */
template<>
struct StreamT<string, char> {
    string const name;
    string const source;
    int row, column;
    size_t count;
    StreamT(string const name, string source): name(name), source(source), row(1), column(1), count(0) {}
    virtual char head() const {
        return source.at(count);
    }
    virtual void next() {
        if (this->head() == '\n') { row += 1, column = 1; }
        else { column += 1; }
        count = count + 1;
    }
    virtual void next(int k) {
        while(count < source.length() && k--) {
            this->next();
        }
    }
    virtual char at(int idx) const {
        return source.at(count + idx);
    }
    virtual pair<int, int> locate() const {
        return ::make_pair(this->row, this->column);
    }
    virtual StreamT tail() const {
        return StreamT(name, source.substr(count + 1));
    }
    virtual StreamT take(int k) const {
        return StreamT(name, source.substr(count, k));
    }
    virtual StreamT drop(int k) const {
        return StreamT(name, source.substr(count + k));
    }
    virtual string extract() const {
        return this->source;
    }
};

/**
 * The result representation of a parser. `Success` and `Failure` are used to construct Value object, while
 * `Aggregate` is used to accmulate two Value object into one.
 */
template<typename V>
struct ValueT {
    string const sname;
    bool const status;
    pair<int, int> loc;
    int len;
    V const actual, expected;

    string name() const {
        return "StreamT " + sname + " @ " + ::to_string(loc.first) + ":" + ::to_string(loc.second)
            + ", " + (status ? "true" : "false") + ", length: " + ::to_string(len) + "\n"
            + "    actual:   " + ::to_string(actual) + "\n"
            + "    expected: " + ::to_string(expected) + "\n";
    }

    ValueT<V>(string const sname, bool status, pair<int, int> loc, int len, V const actual, V const expected)
        : sname(sname), status(status), loc(loc), len(len), actual(actual), expected(expected) {}

    static ValueT<V> Success(string const sname, pair<int, int> loc, int len, V const actual, V const expected) {
        return ValueT<V>(sname, true, loc, len, actual, expected);
    }

    static ValueT<V> Failure(string const sname, pair<int, int> loc, int len, V const actual, V const expected) {
        return ValueT<V>(sname, false, loc, len, actual, expected);
    }

    static ValueT<V> Aggregate(ValueT<V> const a, ValueT<V> const b) {
        if (!a.status) return a;
        if (!b.status) return b;
        return ValueT<V>::Success(b.sname, b.loc, a.len + b.len, a.actual.push_back(b.actual), a.expected.push_back(b.expected));
    }
};

// template<typename V>
struct ParseError: public exception {
    string const sname, message;
    pair<int, int> const loc;
    // V const actual, expected;
    ParseError(string const & sname, string const & message, pair<int, int> const loc): exception(), sname(sname), message(message), loc(loc) {}
    virtual const char *what() const noexcept { return (sname + ": " + message).c_str(); }
};

/**
 * Parser model, wraps a function parse a Value object from a stream at specified position.
 */
template<typename S, typename V>
struct ParsecT {
    constexpr ParsecT<S, V>() {}
    virtual string name() const = 0;
    ValueT<V> parse(S text) const {
        pair<int, int> loc = text.locate();
        pair<int, pair<V, V>> res;
        try { // try exception throwed during operating the stream object.
            res = (*this)(text);
        } catch (std::exception & e) {
            res = ::pair<int, pair<V, V>>(0, pair<V, V>());
        }
        text.next(res.first); // move ahead offset.
        return ValueT<V>(text.name, res.first != 0, loc, res.first, res.second.first, res.second.second);
    }
    // return <offset, <actual, expected>>
    virtual pair<int, pair<V, V>> operator () (S const) const = 0;
};

/**
 * Some parsers for parse characters. (use constexpr for performance improvement)
 */
struct any: public ParsecT<StreamT<string, char>, char> {
    constexpr any(): ParsecT() {}
    string name() const override { return "any character"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(c != EOF ? 1 : 0, make_pair(c, c));
    }
} constexpr any;

struct blank: public ParsecT<StreamT<string, char>, char> {
    constexpr blank(): ParsecT() {}
    string name() const override { return "blank"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isblank(c) ? 1 : 0, make_pair(c, ' '));
    }
} constexpr blank;

struct cntrl: public ParsecT<StreamT<string, char>, char> {
    constexpr cntrl(): ParsecT() {}
    string name() const override { return "cntrl"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::iscntrl(c) ? 1 : 0, make_pair(c, '\x94'));
    }
} constexpr cntrl;

struct space: public ParsecT<StreamT<string, char>, char> {
    constexpr space(): ParsecT() {}
    string name() const override { return "space"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isspace(c) ? 1 : 0, make_pair(c, ' '));
    }
} constexpr space;

struct eof: public ParsecT<StreamT<string, char>, char> {
    constexpr eof(): ParsecT() {}
    string name() const override { return "eof"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(c == EOF ? 1 : 0, make_pair(c, EOF));
    }
} constexpr eof;

struct eol: public ParsecT<StreamT<string, char>, char> {
    constexpr eol(): ParsecT() {}
    string name() const override { return "eol"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(c == '\n' ? 1 : 0, make_pair(c, '\n'));
    }
} constexpr eol;

struct tab: public ParsecT<StreamT<string, char>, char> {
    constexpr tab(): ParsecT() {}
    string name() const override { return "tab"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(c == '\t' ? 1 : 0, make_pair(c, '\t'));
    }
} constexpr tab;

struct punct: public ParsecT<StreamT<string, char>, char> {
    constexpr punct(): ParsecT() {}
    string name() const override { return "punctuation"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isspace(c) ? 1 : 0, make_pair(c, '.'));
    }
} constexpr punct;

struct digit: public ParsecT<StreamT<string, char>, char> {
    constexpr digit(): ParsecT() {}
    string name() const override { return "digit"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isdigit(c) ? 1 : 0, make_pair(c, '0'));
    }
} constexpr digit;

struct xdigit: public ParsecT<StreamT<string, char>, char> {
    constexpr xdigit(): ParsecT() {}
    string name() const override { return "xdigit"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isxdigit(c) ? 1 : 0, make_pair(c, 'x'));
    }
} constexpr xdigit;

struct upper: public ParsecT<StreamT<string, char>, char> {
    constexpr upper(): ParsecT() {}
    string name() const override { return "upper"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isupper(c) ? 1 : 0, make_pair(c, 'u'));
    }
} constexpr upper;

struct lower: public ParsecT<StreamT<string, char>, char> {
    constexpr lower(): ParsecT() {}
    string name() const override { return "lower"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::islower(c) ? 1 : 0, make_pair(c, 'l'));
    }
} constexpr lower;

struct alpha: public ParsecT<StreamT<string, char>, char> {
    constexpr alpha(): ParsecT() {}
    string name() const override { return "alpha"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isalpha(c) ? 1 : 0, make_pair(c, 'a'));
    }
} constexpr alpha;

struct alnum: public ParsecT<StreamT<string, char>, char> {
    constexpr alnum(): ParsecT() {}
    string name() const override { return "alnum"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isalnum(c) ? 1 : 0, make_pair(c, 'n'));
    }
} constexpr alnum;

struct print: public ParsecT<StreamT<string, char>, char> {
    constexpr print(): ParsecT() {}
    string name() const override { return "print"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isprint(c) ? 1 : 0, make_pair(c, 'p'));
    }
} constexpr print;

struct graph: public ParsecT<StreamT<string, char>, char> {
    constexpr graph(): ParsecT() {}
    string name() const override { return "graph"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(::isgraph(c) ? 1 : 0, make_pair(c, 'g'));
    }
} constexpr graph;

class character: public ParsecT<StreamT<string, char>, char> {
    char _c;
public:
    character(): ParsecT(), _c('\0') {}
    string name() const override { return "character '" + string(1, _c) + "'"; }
    virtual character operator () (char const c) { _c = c; return *this; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const text) const override {
        char c = text.head();
        return make_pair(_c == c ? 1 : 0, make_pair(c, _c));
    }
} character;

class string_literal: public ParsecT<StreamT<string, char>, string> {
    string _s;
public:
    string_literal(): ParsecT(), _s("\0") {}
    string name() const override { return "string \"" + _s + "\""; }
    virtual string_literal operator () (string const s) { _s = s; return *this; }
    virtual pair<int, pair<string, string>> operator () (StreamT<string, char> const text) const override {
        string s = text.take(_s.length()).extract();
        return make_pair(_s == s ? s.length() : 0, make_pair(s, _s));
    }
} string_literal;



