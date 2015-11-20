/********************************************************
 * libparsec
 *
 * Description:     A univeral C++ parser combinator library 
 *                  inspirted by Parsec library of Haskell.
 * Author:          He Tao
 *
 *********************************************************/

#ifndef __PARSEC_HPP__
#define __PARSEC_HPP__

#include <iostream>
#include <string>
#include <functional>
#include <exception>
#include <vector>
#include <cstring>
using namespace std;

// data type: Maybe
template<typename T>
struct Maybe{
private:
    bool const nil;
    T const val;
public:
    using data_type = T;
    constexpr Maybe(): nil(true) {}
    constexpr Maybe(T const & val): val(val), nil(false) {}
    bool Nil() const { return this->nil; }
    T Val() const { if (this->nil) { throw val; } else { return val; } }
    constexpr operator bool () const { return this->nil == false; }
};
template<typename T>
constexpr Maybe<T> const Just(T const & val) {
    return Maybe<T>(val);
}
template<typename T>
constexpr Maybe<T> const Nothing() {
    return Maybe<T>();
}
template<typename T>
constexpr T const FromJust(Maybe<T> const & wrapper) {
    return wrapper.Val();
}

// Input stream model.

struct input_t {
private:
    string const & desc;
    string const text;
public:
    input_t(string const text, string const & desc = "unknown input stream"): desc(desc), text(text) {}
    ~input_t() {}
    input_t *next(int const & k) { return this->drop(k); }
    bool empty() const { return this->text.empty(); }
    char at(int const & k) { return this->text.at(k); }
    input_t *drop(int const & k) {
        if (k < 0) {
            // cout <<  string("ERROR: try to drop ") << k << " chars from input stream." << endl;
            return this;
        }
        if ((size_t)k < this->text.length()) {
            return new input_t(this->text.substr(k), this->desc);
        }
        return new input_t("", this->desc);
    }
    string take(int const & k) {
        if ((size_t)k <= this->text.length()) {
            return this->text.substr(0, k);
        }
        return string();
    }
    int length() { return this->text.length(); }
    string extract() { cout << "this->text: " << this->text << endl; return this->text; }

    // overload stringstream operator.
    friend std::ostream & operator << (std::ostream & out, input_t const & t) {
        out << t.text << endl;
        return out;
    }
};

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
    bool strict;

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

    ValueT<V>(bool status, pair<int, int> loc, int len, V const & actual, string const & expected, bool const & strict = false)
        : status(status), loc(loc), len(len), actual(actual), expected(expected), strict(strict) {}

    static ValueT<V> Success(pair<int, int> loc, int len, V const & actual, string const & expected, bool const & strict = false) {
        return ValueT<V>(true, loc, len, actual, expected, strict);
    }

    static ValueT<V> Failure(pair<int, int> loc, int len, V const & actual, string const & expected, bool const & strict = false) {
        return ValueT<V>(false, loc, len, actual, expected, strict);
    }
};


struct ParseError: public exception {
    string const sname, message;
    pair<int, int> const loc;
    ParseError(string const & sname, string const & message, pair<int, int> const loc): exception(), sname(sname), message(message), loc(loc) {}
    const char *what() const noexcept { return (sname + ": " + message).c_str(); }
};


/**
 * Parser model, wraps a function parse a Value object from a stream at specified position.
 */

template<typename P, typename = typename P::parser_type>
struct ParsecT {
private:
    P const parser;
public:
    using V = typename P::parser_type;
    constexpr ParsecT(P const & parser): parser(parser) {}
    string name() const { return "ParsecT"; }
    ValueT<V> parse(input_t *text) const {
        pair<int, int> loc = make_pair(1, 1);
        tuple<int, V, string> res;
        try { // try exception throwed during operating the stream object.
            res = parser(text);
        } catch (std::exception & e) {
            cout << string("!!!Exception: ") + string(e.what()) << endl;
            res = make_tuple(-1, V(), "!!!Exception: " + string(e.what()));
        } catch (...) {
            cout << string("!!!Exception: ") + "unknown exception." << endl;
        }
        text = text->next(std::get<0>(res)); // move ahead offset.
        if (!text->empty()) {
            // cout << "NOT COMSUME ALL TOKEN IN THE INPUT STREAM!" << endl;
            // cout << "    in stream: " << (*text) << endl;
        }
        return ValueT<V>(std::get<0>(res) != -1, loc, std::get<0>(res), std::get<1>(res), std::get<2>(res), text->empty());
    }
    ValueT<V> operator () (input_t *text) const {
        return this->parse(text);
    }
};

template<typename T>
class parser_t {
private:
    pair<int, int> loc;
    string const desc;
    function<pair<int, T>(input_t *)> fn;
public:
    using parser_type = T;
    parser_t(string const desc): desc(desc) {}
    parser_t(string const desc, function<pair<int, T>(input_t *)> fn): desc(desc), fn(fn) {}
    ~parser_t() {}
    void setfn(function<pair<int, T>(input_t *)> const & fn) {
        this->fn = fn;
    }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res = this->fn(text);
        return make_tuple(res.first, res.second, this->name());
    }
    string const name() const { return this->desc; }
};

/**
 * Parser combinators.
 */

template<typename PA, typename PB, typename = typename PA::parser_type, typename = typename PB::parser_type>
class compose {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PB::parser_type;
    constexpr compose(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "compose two parser."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res1 = pa(text);
        if (std::get<0>(res1) == -1) {
            return make_tuple(-1, parser_type(), name());
        }
        auto res2 = pb(text->drop(std::get<0>(res1)));
        if (std::get<0>(res2) == -1) {
            return make_tuple(-1, parser_type(), name());
        }
        else {
            return make_tuple(std::get<0>(res1) + std::get<0>(res2), std::get<1>(res2), name());
        }
    }
};
// operator '>>'
template<typename PA, typename PB>
constexpr compose<PA, PB> const operator >> (PA const & pa, PB const & pb) {
    return compose<PA, PB>(pa, pb);
}

template<typename PA, typename PB, typename = typename PA::parser_type, typename = typename PB::parser_type>
class choice {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr choice(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "choice from two parser."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res1 = pa(text);
        if (std::get<0>(res1) != -1) {
            return make_tuple(std::get<0>(res1), std::get<1>(res1), name());
        }
        else {
            auto res2 = pb(text);
            return make_tuple(std::get<0>(res2), std::get<1>(res2), name());
        }
    }
};
// operator '|'
template<typename PA, typename PB>
constexpr choice<PA, PB> const operator | (PA const & pa, PB const & pb) {
    return choice<PA, PB>(pa, pb);
}

template<typename PA, typename PB, typename = typename PA::parser_type, typename = typename PB::parser_type>
class try_choice {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr try_choice(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "try choice from two parser with backtracking."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res1 = pa(text);
        if (res1.first != -1) {
            return make_tuple(std::get<0>(res1), std::get<1>(res1), name());
        }
        else {
            auto res2 = pb(text);
            return make_tuple(std::get<0>(res2), std::get<1>(res2), name());
        }
    }
};
// operator '^'
template<typename PA, typename PB>
constexpr try_choice<PA, PB> const operator ^ (PA const & pa, PB const & pb) {
    return try_choice<PA, PB>(pa, pb);
}

template<typename PA, typename PB, typename = typename PA::parser_type, typename = typename PB::parser_type>
class joint {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = pair<typename PA::parser_type, typename PB::parser_type>;
    constexpr joint(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "joint two parser."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        string expected;
        auto res1 = pa(text);
        expected += std::get<2>(res1) + ", ";
        if (std::get<0>(res1) == -1) {
            return make_tuple(-1, parser_type(), name());
        }
        auto res2 = pb(text->drop(std::get<0>(res1)));
        expected += std::get<2>(res2) + ", ";
        if (std::get<0>(res2) == -1) {
            return make_tuple(-1, parser_type(), name());
        }
        else {
            return make_tuple(std::get<0>(res1) + std::get<0>(res2), make_pair(std::get<1>(res1), std::get<1>(res2)), expected);
        }
    }
};
// operator '+'
template<typename PA, typename PB>
constexpr joint<PA, PB> const operator + (PA const & pa, PB const & pb) {
    return joint<PA, PB>(pa, pb);
}

template<typename PA, typename PB, typename = typename PA::parser_type, typename = typename PB::parser_type>
class except {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr except(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "except one from two parsers."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res1 = pa(text);
        auto res2 = pb(text);
        if (std::get<0>(res1) != -1 && std::get<0>(res2) == -1) {
            return res1;
        }
        return make_tuple(-1, parser_type(), "except " + std::get<2>(res1) + " from " + std::get<2>(res2));
    }
};
// operator '-'
template<typename PA, typename PB>
constexpr except<PA, PB> const operator - (PA const & pa, PB const & pb) {
    return except<PA, PB>(pa, pb);
}

template<typename PA, typename PB, typename = typename PA::parser_type, typename = typename PB::parser_type>
class skip {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr skip(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "skip the second parser."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res1 = pa(text);
        if (std::get<0>(res1) == -1) {
            return make_tuple(-1, parser_type(), name());
        }
        auto res2 = pb(text->drop(std::get<0>(res1)));
        if (std::get<0>(res2) == -1) {
            return make_tuple(-1, parser_type(), name());
        }
        // the `skip` combinator DOES consume the end flag. see also `ends_with` combinator.
        return make_tuple(std::get<0>(res1) + std::get<0>(res2), std::get<1>(res1), name());
    }
};
// operator '<<'
template<typename PA, typename PB>
constexpr skip<PA, PB> const operator << (PA const & pa, PB const & pb) {
    return skip<PA, PB>(pa, pb);
}

template<typename PA, typename PB, typename = typename PA::parser_type, typename = typename PB::parser_type>
class ends_with {
private:
    PA const pa;
    PB const pb;
public:
    using parser_type = typename PA::parser_type;
    constexpr ends_with(PA const & pa, PB const & pb): pa(pa), pb(pb) {}
    string name() const { return "ends with the second parser."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res1 = pa(text);
        if (std::get<0>(res1) == -1) {
            return make_tuple(-1, parser_type(), name());
        }
        auto res2 = pb(text->drop(std::get<0>(res1)));
        if (std::get<0>(res2) == -1) {
            return make_tuple(-1, parser_type(), name());
        }
        else {
            // the `ends_with` combinator DOES NOT consume the end flag. see also `skip` combinator.
            return make_tuple(std::get<0>(res1), std::get<1>(res1), name());
        }
    }
};
// operator '<'
template<typename PA, typename PB>
constexpr ends_with<PA, PB> const operator < (PA const & pa, PB const & pb) {
    return ends_with<PA, PB>(pa, pb);
}

template<typename PA, typename = typename PA::parser_type>
class times {
private:
    PA const pa;
    int const mint, maxt;
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr times(PA const & pa, int const & mint, int const & maxt): pa(pa), mint(mint), maxt(maxt) {}
    string name() const { return "repeat a parser many times."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        int cnt = 0, offset = 0;
        parser_type actual;
        string expected;
        while (cnt < maxt) {
            auto res = pa(text);
            if (get<0>(res) == -1) {
                if (cnt < mint) {
                    offset = -1; // failed.
                }
                break;
            }
            cnt = cnt + 1; // update counter.
            offset = offset + std::get<0>(res); // update length.
            actual.push_back(std::get<1>(res));
            expected += std::get<2>(res) + ", ";
            text = text->drop(std::get<0>(res)); // update text stream.
            if (cnt >= maxt) {
                break;
            }
        }
        return make_tuple(offset, actual, expected);
    }
};

template<typename PA, typename = typename PA::parser_type>
struct optional {
private:
    PA const pa;
public:
    using parser_type = typename PA::parser_type;
    constexpr optional(PA const & pa): pa(pa) {}
    string name() const { return "optional: repeat 0 or 1 times."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res = pa(text);
        if (std::get<0>(res) == -1) {
            return make_tuple(0, parser_type(), name());
        }
        else {
            return res;
        }
    }
};
// operator '~'
template<typename PA>
constexpr optional<PA> const operator ~ (PA const & pa) {
    return optional<PA>(pa);
}

template<typename PA, typename = typename PA::parser_type>
struct many {
private:
    PA const pa;
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr many(PA const & pa): pa(pa) {}
    string name() const { return "many: repeat 0 or more times."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        return times<PA>(pa, 0, 0x7fffffff)(text);
    }
};
// operator '++()'
template<typename PA>
constexpr many<PA> const operator ++ (PA const & pa) {
    return many<PA>(pa);
}

template<typename PA, typename = typename PA::parser_type>
struct many1 {
private:
    PA const pa;
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr many1(PA const & pa): pa(pa) {}
    string name() const { return "many1: repeat 1 or more times."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        return times<PA>(pa, 1, 0x7fffffff)(text);
    }
};
// operator '()++'
template<typename PA>
constexpr many1<PA> const operator ++ (PA const & pa, int) {
    return many1<PA>(pa);
}

template<typename PA, typename = typename PA::parser_type>
struct countk {
private:
    PA const pa;
    int const k;
public:
    using parser_type = vector<typename PA::parser_type>;
    constexpr countk(PA const & pa, const int & k): pa(pa), k(k) {}
    string name() const { return "countk: repeat exactly k times."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        return times<PA>(pa, k, k)(text);
    }
};
// operator '*'
template<typename PA>
constexpr countk<PA> const operator * (PA const & pa, int const & k) {
    return countk<PA>(pa, k);
}

// sepby1 p sep parses one or more occurrences of p, separated by sep. Returns a vector of values returned by p.
template<typename P, typename SEP, typename = typename P::parser_type, typename = typename SEP::parser_type>
struct sepby1 {
private:
    P const p;
    SEP const sep;
public:
    using parser_type = vector<typename P::parser_type>;
    constexpr sepby1(P const & p, SEP const & sep): p(p), sep(sep) {}
    string name() const { return "one or more occurrences of p, separated by sep."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res = (p + (++(sep >> p)))(text);
        string description = std::get<2>(res);
        parser_type vec = std::get<1>(res).second;
        auto firstp = std::get<1>(res).first;
        vec.insert(vec.begin(), firstp);
        return make_tuple(std::get<0>(res), vec, description);
    }
};
// operator '%'
template<typename P, typename SEP, typename = typename P::parser_type, typename = typename SEP::parser_type>
constexpr sepby1<P, SEP> const operator % (P const & p, SEP const & sep) {
    return sepby1<P, SEP>(p, sep);
}

template<typename PA, typename T, typename = typename PA::parser_type>
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
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto res = pa(text);
        return make_tuple(std::get<0>(res), fn(std::get<1>(res)), "map fn on " + name());
    }
};
// operator '/'
template<typename PA, typename T>
constexpr mapfn<PA, T> const operator / (PA const & pa, function<T(typename PA::parser_type)> const & fn) {
    return mapfn<PA, T>(pa, fn);
}

// `chainl p op x` parser one or more occurrences of p, separated by op Returns a value
// obtained by a left associative application of all functions returned by op to the values 
// returned by p. . This parser can for example be used to eliminate **left recursion** which 
// typically occurs in expression grammars.
template<typename P, typename OP, typename = typename P::parser_type, typename = typename OP::parser_type>
struct chainl {
private:
    P const p;
    OP const op;
public:
    using element_type = ::pair<typename OP::parser_type, typename P::parser_type>;
    using parser_type = vector<element_type>;
    constexpr chainl(P const & p, OP const & op): p(p), op(op) {}
    string name() const { return "chainl combinator, to handle left-recursive grammars."; }
    tuple<int, parser_type const, string const> operator () (input_t *text) const {
        auto result = parser_type();
        int length = 0;
        
        auto res1 = p(text);
        if (std::get<0>(res1) == -1) { // parser the first left value.
            return make_tuple(-1, result, name());
        }
        length = std::get<0>(res1);
        text = text->drop(std::get<0>(res1));
        element_type e = element_type();
        e.second = std::get<1>(res1);
        result.emplace_back(e);
        
        // parser the other values.
        while (!text->empty()) {
            auto res = (op + p)(text);
            if (std::get<0>(res) == -1) {
                break;
            }
            length += std::get<0>(res);
            text = text->drop(std::get<0>(res));
            result.emplace_back(std::get<1>(res));
        }

        // finish.
        return make_tuple(length, result, name());
    }
};
// operator ">="
template<typename P, typename OP>
constexpr chainl<P, OP> const operator >= (P const & p, OP const & op) {
    return chainl<P, OP>(p, op);
}

// chainr p op x parser one or more occurrences of |p|, separated by op Returns a value obtained by a 
// **right associative** application of all functions returned by op to the values returned by p.
template<typename P, typename OP, typename = typename P::parser_type, typename = typename OP::parser_type>
struct chainr {
    // TODO: not implemented.
private:
    P const p;
    OP const op;
public:
    using element_type = ::pair<typename OP::parser_type, typename P::parser_type>;
    using parser_type = vector<element_type>;
    constexpr chainr(P const & p, OP const & op): p(p), op(op) {}
    string name() const { return "chainl combinator, to handle right-recursive grammars."; }
    tuple<int, parser_type const, string const> operator () (string const & text) const {
        auto result = parser_type();
        string s = text;
        int length = 0;
        
        auto res1 = p(s);
        if (res1.first == -1) { // parser the first left value.
            return make_pair(-1, result, name());
        }
        length = res1.first;
        s = drop(s, res1.first);
        element_type e = element_type();
        e.second = res1.second.first;
        result.emplace_back(e);
        
        // parser the other values.
        while (!s.empty()) {
            auto res = (op + p)(s);
            if (std::get<0>(res) == -1) {
                break;
            }
            length += std::get<0>(res);
            s = drop(s, std::get<0>(res));
            result.emplace_back(res.second.first);
        }

        // finish.
        return make_tuple(length, result, name());
    }
};
// operator "<="
template<typename P, typename OP>
constexpr chainr<P, OP> const operator <= (P const & p, OP const & op) {
    return chainr<P, OP>(p, op);
}


/**
 * Some parsers for parse characters. (use constexpr for performance improvement)
 */

pair<int, char> any_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(c != EOF ? (1) : (-1), c);
}
const class parser_t<char> any("any character", any_fn);

pair<int, char> blank_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isblank(c) ? (1) : (-1), c);
}
const class parser_t<char> blank("blank", blank_fn);

pair<int, char> cntrl_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::iscntrl(c) ? (1) : (-1), c);
}
const class parser_t<char> cntrl("cntrl", cntrl_fn);

pair<int, char> space_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isspace(c) ? (1) : (-1), c);
}
const class parser_t<char> space("space", space_fn);

pair<int, char> end_f_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(c == EOF ? (1) : (-1), c);
}
const class parser_t<char> end_f("eof", end_f_fn);

pair<int, char> eol_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(c == '\n' ? (1) : (-1), c);
}
const class parser_t<char> eol("eol", eol_fn);

pair<int, char> tab_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(c == '\t' ? (1) : (-1), c);
}
const class parser_t<char> tab("tab", tab_fn);

pair<int, char> digit_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isdigit(c) ? (1) : (-1), c);
}
const class parser_t<char> digit("digit", digit_fn);

pair<int, char> xdigit_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isxdigit(c) ? (1) : (-1), c);
}
const class parser_t<char> xdigit("xdigit", xdigit_fn);

pair<int, char> upper_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isupper(c) ? (1) : (-1), c);
}
const class parser_t<char> upper("upper", upper_fn);

pair<int, char> lower_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::islower(c) ? (1) : (-1), c);
}
const class parser_t<char> lower("lower", lower_fn);

pair<int, char> alpha_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isalpha(c) ? (1) : (-1), c);
}
const class parser_t<char> alpha("alpha", alpha_fn);

pair<int, char> alnum_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isalnum(c) ? (1) : (-1), c);
}
const class parser_t<char> alnum("alnum", alnum_fn);

pair<int, char> print_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isprint(c) ? (1) : (-1), c);
}
const class parser_t<char> print("print", print_fn);

pair<int, char> graph_fn(input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    return make_pair(::isgraph(c) ? (1) : (-1), c);
}
const class parser_t<char> graph("graph", graph_fn);


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
} constexpr spaces;



#endif /* __PARSEC_HPP__ */
