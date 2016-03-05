#ifndef __PARSE_TOOL_HPP__
#define __PARSE_TOOL_HPP__

#include <vector>
#include <functional>
#include "patch.hpp"
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

    string str(int n) const { return to_string(n); }
    string str(char c) const { return string(1, c); }
    string str(string s) const { return s; }
    string str(vector<char> vec) const { string ans = "vector char: "; for (char c: vec) { ans.push_back(c); ans.push_back(','); } return ans; }
    string str(vector<string> vec) const { string ans = "vector string: "; for (string s: vec) { ans += s + ","; } return ans; }

    string name() const {
        return "stream @ " + to_string(loc.first) + ":" + to_string(loc.second)
            + ", " + (status ? "true" : "false") + ", length: " + to_string(len) + "\n"
            + "    expected: " + str(expected) + "\n";
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


#endif /* __PARSE_TOOL_HPP__ */
