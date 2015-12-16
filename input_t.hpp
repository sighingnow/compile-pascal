#ifndef __INPUT_T_HPP__
#define __INPUT_T_HPP__

#include <iostream>
#include <string>
#include "patch.hpp"

using namespace std;

// Input stream model.

struct input_t {
private:
    string const & desc;
    int line, column;
    string text;
public:
    input_t(string const text, int line = 1, int column = 1, string const & desc = "unknown input stream"): desc(desc), line(line), column(column), text(text) {}
    ~input_t() {}
    input_t *next(int const & k) { return this->drop(k); }
    bool empty() const { return this->text.empty(); }
    char at(int const & k) { return this->text.at(k); }
    input_t *drop(int const & k) {
        if (k < 0) {
            // cout <<  string("ERROR: try to drop ") << k << " chars from input stream." << endl;
            return this;
        }
        int l = line, c = column;
        size_t len = min((size_t)k, text.length());
        for (size_t i = 0; i < len; ++i) {
            if (text.at(i) == '\n') {
                l = l + 1; c = 1;
            }
            else {
                c = c + 1;
            }
        }
        if ((size_t)k < this->text.length()) {
            return new input_t(this->text.substr(k), l, c, this->desc);
        }
        return new input_t("", l, c, this->desc);
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

    // get current location of the stream.
    pair<int, int> locate() {
        return make_pair(this->line, this->column);
    }
};

#endif /* __INPUT_T_HPP__ */
