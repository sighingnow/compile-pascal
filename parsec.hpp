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
    virtual bool empty() = 0;
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
    virtual bool empty() const {
        return source.empty();
    }
    virtual char head() const {
        return source.at(count);
    }
    virtual void next() {
        if (this->head() == '\n') { row += 1, column = 1; }
        else { column += 1; }
        count = count + 1;
    }
    virtual void next(int k) {
        while(count < source.length() && k--> 0) {
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
            res = ::pair<int, pair<V, V>>(-1, pair<V, V>());
        }
        text.next(res.first); // move ahead offset.
        return ValueT<V>(text.name, res.first != -1, loc, res.first, res.second.first, res.second.second);
    }
    // return <offset, <actual, expected>>
    virtual pair<int, pair<V, V>> operator () (S const &) const = 0;
};



/**
 * Some parsers for parse characters. (use constexpr for performance improvement)
 */
struct any: public ParsecT<StreamT<string, char>, char> {
    constexpr any(): ParsecT() {}
    string name() const override { return "any character"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(c != EOF ? (1) : (-1), make_pair(c, c));
    }
} constexpr any;

struct blank: public ParsecT<StreamT<string, char>, char> {
    constexpr blank(): ParsecT() {}
    string name() const override { return "blank"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isblank(c) ? (1) : (-1), make_pair(c, ' '));
    }
} constexpr blank;

struct cntrl: public ParsecT<StreamT<string, char>, char> {
    constexpr cntrl(): ParsecT() {}
    string name() const override { return "cntrl"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::iscntrl(c) ? (1) : (-1), make_pair(c, '\x94'));
    }
} constexpr cntrl;

struct space: public ParsecT<StreamT<string, char>, char> {
    constexpr space(): ParsecT() {}
    string name() const override { return "space"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isspace(c) ? (1) : (-1), make_pair(c, ' '));
    }
} constexpr space;

struct eof: public ParsecT<StreamT<string, char>, char> {
    constexpr eof(): ParsecT() {}
    string name() const override { return "eof"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(c == EOF ? (1) : (-1), make_pair(c, EOF));
    }
} constexpr eof;

struct eol: public ParsecT<StreamT<string, char>, char> {
    constexpr eol(): ParsecT() {}
    string name() const override { return "eol"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(c == '\n' ? (1) : (-1), make_pair(c, '\n'));
    }
} constexpr eol;

struct tab: public ParsecT<StreamT<string, char>, char> {
    constexpr tab(): ParsecT() {}
    string name() const override { return "tab"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(c == '\t' ? (1) : (-1), make_pair(c, '\t'));
    }
} constexpr tab;

struct punct: public ParsecT<StreamT<string, char>, char> {
    constexpr punct(): ParsecT() {}
    string name() const override { return "punctuation"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isspace(c) ? (1) : (-1), make_pair(c, '.'));
    }
} constexpr punct;

struct digit: public ParsecT<StreamT<string, char>, char> {
    constexpr digit(): ParsecT() {}
    string name() const override { return "digit"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isdigit(c) ? (1) : (-1), make_pair(c, '0'));
    }
} constexpr digit;

struct xdigit: public ParsecT<StreamT<string, char>, char> {
    constexpr xdigit(): ParsecT() {}
    string name() const override { return "xdigit"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isxdigit(c) ? (1) : (-1), make_pair(c, 'x'));
    }
} constexpr xdigit;

struct upper: public ParsecT<StreamT<string, char>, char> {
    constexpr upper(): ParsecT() {}
    string name() const override { return "upper"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isupper(c) ? (1) : (-1), make_pair(c, 'u'));
    }
} constexpr upper;

struct lower: public ParsecT<StreamT<string, char>, char> {
    constexpr lower(): ParsecT() {}
    string name() const override { return "lower"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::islower(c) ? (1) : (-1), make_pair(c, 'l'));
    }
} constexpr lower;

struct alpha: public ParsecT<StreamT<string, char>, char> {
    constexpr alpha(): ParsecT() {}
    string name() const override { return "alpha"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isalpha(c) ? (1) : (-1), make_pair(c, 'a'));
    }
} constexpr alpha;

struct alnum: public ParsecT<StreamT<string, char>, char> {
    constexpr alnum(): ParsecT() {}
    string name() const override { return "alnum"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isalnum(c) ? (1) : (-1), make_pair(c, 'n'));
    }
} constexpr alnum;

struct print: public ParsecT<StreamT<string, char>, char> {
    constexpr print(): ParsecT() {}
    string name() const override { return "print"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isprint(c) ? (1) : (-1), make_pair(c, 'p'));
    }
} constexpr print;

struct graph: public ParsecT<StreamT<string, char>, char> {
    constexpr graph(): ParsecT() {}
    string name() const override { return "graph"; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(::isgraph(c) ? (1) : (-1), make_pair(c, 'g'));
    }
} constexpr graph;

class character: public ParsecT<StreamT<string, char>, char> {
    char _c;
public:
    character(): ParsecT(), _c('\0') {}
    string name() const override { return "character '" + string(1, _c) + "'"; }
    virtual character operator () (char const c) { _c = c; return *this; }
    virtual pair<int, pair<char, char>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<char, char>()); }
        char c = text.head();
        return make_pair(_c == c ? (1) : (-1), make_pair(c, _c));
    }
} character;

class string_literal: public ParsecT<StreamT<string, char>, string> {
    string _s;
public:
    string_literal(): ParsecT(), _s("\0") {}
    string name() const override { return "string \"" + _s + "\""; }
    virtual string_literal operator () (string const s) { _s = s; return *this; }
    virtual pair<int, pair<string, string>> operator () (StreamT<string, char> const & text) const override {
        if (text.empty()) { return make_pair(-1, ::pair<string, string>()); }
        string s = text.take(_s.length()).extract();
        return make_pair(_s == s ? s.length() : (-1), make_pair(s, _s));
    }
} string_literal;

/**
 * Parser combinators.
 */
template <typename S, typename A, typename B, typename V>
class Combinator: public ParsecT<S, V> {
protected:
    ParsecT<S, A> const *pa;
    ParsecT<S, B> const *pb;
public:
    constexpr Combinator(ParsecT<S, A> const & pa, ParsecT<S, B> const & pb): ParsecT<S, V>(), pa(&pa), pb(&pb) {}
    virtual string name() const override = 0;
    virtual pair<int, pair<V, V>> operator () (S const &) const override = 0;
};

template<typename V>
V mempty() { throw ParseError("StreamT", "Mempty method not implement.", make_pair(-1, -1)); }
template<>
char mempty<char>() { return '\0'; }
template<>
string mempty<string>() { return ""; }

/**
 * Merge two values(with type A and type B) into one(with type V).
 */
template<typename V, typename A, typename B>
V merge(A const &, B const &) { throw ParseError("StreamT", "Merge method not implement.", make_pair(-1, -1)); }
template<>
string merge<string, char, char>(char const & a, char const & b) { return string(1, a) + string(1, b); }
template<>
string merge<string, string, char>(string const & s, char const & c) { return s + string(1, c); }
template<>
string merge<string, string, string>(string const & a, string const & b) { return a + b; }

/**
 * Parser combinator instances.
 */

template <typename S, typename A, typename B, typename V>
struct compose: public Combinator<S, A, B, V> {
    constexpr compose(ParsecT<S, A> const & pa, ParsecT<S, B> const & pb): Combinator<S, A, B, V>(pa, pb) {}
    virtual string name() const override { return "compose"; }
    virtual pair<int, pair<V, V>> operator () (S const & text) const override {
        auto res1 = (*(this->pa))(text);
        auto res2 = (*(this->pb))(text.drop(res1.first));
        if (res1.first == -1) {
            return res1;
        }
        if (res2.first == -1) {
            return res2;
        }
        return make_pair(res1.first + res2.first, res2.second);
    }
};

template <typename S, typename A, typename B, typename V>
struct choice: public Combinator<S, A, B, V> {
    constexpr choice(ParsecT<S, A> const & pa, ParsecT<S, B> const & pb): Combinator<S, A, B, V>(pa, pb) {}
    virtual string name() const override { return "choice"; }
    virtual pair<int, pair<V, V>> operator () (S const & text) const override {
        auto res1 = (*(this->pa))(text);
        if (res1.first != -1) {
            return res1;
        }
        auto res2 = (*(this->pb))(text); // TODO: now the `choice` combinator just with backtracking, as same as try_choice.
        return res2;
    }
};

template <typename S, typename A, typename B, typename V>
struct try_choice: public Combinator<S, A, B, V> {
    constexpr try_choice(ParsecT<S, A> const & pa, ParsecT<S, B> const & pb): Combinator<S, A, B, V>(pa, pb) {}
    virtual string name() const override { return "try_choice"; }
    virtual pair<int, pair<V, V>> operator () (S const & text) const override {
        auto res1 = (*(this->pa))(text);
        if (res1.first != -1) {
            return res1;
        }
        auto res2 = (*(this->pb))(text); // try with backtracking.
        return res2;
    }
};

template <typename S, typename A, typename B, typename V>
struct joint: public Combinator<S, A, B, V> {
    constexpr joint(ParsecT<S, A> const & pa, ParsecT<S, B> const & pb): Combinator<S, A, B, V>(pa, pb) {}
    virtual string name() const override { return "joint"; }
    virtual pair<int, pair<V, V>> operator () (S const & text) const override {
        auto res1 = (*(this->pa))(text);
        auto res2 = (*(this->pb))(text.drop(res1.first));
        V actual = merge<V, A, B>(res1.second.first, res2.second.first);
        V expected = merge<V, A, B>(res1.second.second, res2.second.second); 
        if (res1.first == -1 || res2.first == -1) {
            return make_pair(-1, make_pair(actual, expected));
        }
        else {
            return make_pair(res1.first + res2.first, make_pair(actual, expected));
        }
    }
};

template <typename S, typename A, typename B, typename V>
struct skip: public Combinator<S, A, B, V> {
    constexpr skip(ParsecT<S, A> const & pa, ParsecT<S, B> const & pb): Combinator<S, A, B, V>(pa, pb) {}
    virtual string name() const override { return "skip"; }
    virtual pair<int, pair<V, V>> operator () (S const & text) const override {
        auto res1 = (*(this->pa))(text);
        if (res1.first == -1) {
            return res1;
        }
        auto res2 = (*(this->pb))(text.drop(res1.first));
        if (res2.first == -1) {
            return res2;
        }
        // the `skip` combinator DOES consume the end flag. see also `ends_with` combinator.
        return make_pair(res1.first + res2.first, res1.second);
    }
};

template <typename S, typename A, typename B, typename V>
struct ends_with: public Combinator<S, A, B, V> {
    constexpr ends_with(ParsecT<S, A> const & pa, ParsecT<S, B> const & pb): Combinator<S, A, B, V>(pa, pb) {}
    virtual string name() const override { return "ends_with"; }
    virtual pair<int, pair<V, V>> operator () (S const & text) const override {
        auto res1 = (*(this->pa))(text);
        if (res1.first == -1) {
            return res1;
        }
        auto res2 = (*(this->pb))(text.drop(res1.first));
        if (res2.first == -1) {
            return res2;
        }
        // the `ends_with` combinator DOES NOT consume the end flag. see also `skip` combinator.
        return res1;
    }
};

/**
 * Repeat combinators.
 */

template <typename S, typename A, typename B, typename V>
struct times: public Combinator<S, A, B, V> {
private:
    int const mint, maxt;
public:
    constexpr times(ParsecT<S, A> const & pa, int const & mint, int const & maxt = 0): Combinator<S, A, B, V>(pa, pa), mint(mint), maxt(maxt) {}
    virtual string name() const override { return "times: repeat " + ::to_string(mint) + " - " + ::to_string(maxt) + " times."; }
    virtual pair<int, pair<V, V>> operator () (S const & text) const override {
        int cnt = 0;
        S const *sp = &text;
        int offset = 0;
        V actual = mempty<V>();
        V expected = mempty<V>();
        while (cnt < maxt) {
            auto res = (*(this->pa))(*sp);
            if (res.first != -1) {
                cnt = cnt + 1; // update counter.
                offset = offset + res.first; // update length.
                actual = merge<V, V, A>(actual, res.second.first); // update actual value.
                expected = merge<V, V, A>(expected, res.second.second); // update expected value.
                S const spt = sp->drop(res.first);
                sp = &spt; // update stream head.
            }
            else {
                if (cnt < mint) {
                    offset = -1; // failed.
                }
                break;
            }
            if (cnt >= maxt) {
                break; // reach to the maximum repeat times.
            }
        }
        return make_pair(offset, make_pair(actual, expected));
    }
};

template <typename S, typename A, typename B, typename V>
struct many: public times<S, A, B, V> {
public:
    constexpr many(ParsecT<S, A> const & pa): times<S, A, B, V>(pa, 0, 0x7fffffff) {}
    virtual string name() const override { return "many: repeat 0 or more times."; }
};

template <typename S, typename A, typename B, typename V>
struct many1: public times<S, A, B, V> {
public:
    constexpr many1(ParsecT<S, A> const & pa): times<S, A, B, V>(pa, 1, 0x7fffffff) {}
    virtual string name() const override { return "many1: repeat 1 or more times."; }
};

template <typename S, typename A, typename B, typename V>
struct countk: public times<S, A, B, V> {
public:
    constexpr countk(ParsecT<S, A> const & pa, const int & k): times<S, A, B, V>(pa, k, k) {}
    virtual string name() const override { return "countk: repeat exactly k times."; }
};


