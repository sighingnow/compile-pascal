libparsec
=========

A univeral C++ parser combinator library inspirted by Parsec library of Haskell.

Overview
--------

A parser type is a struct that have the field called `parser_type`, a typical example is:

```cpp
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
```

The `parser_type` is the type of a value that the parser can parse from the given source text.

A parser is an object that take a string as input ans return a nested pair as follow:

    std::pair<length, std::pair<actual, expected>>

+ length

    The length of text that the parser consumes. If length is -1, indicates that the parser can't
    recognise the given text.

+ actual

    The actual value of the parser can parse from given text.

+ expected

    The description of the parser's expected value.

A combinator is an function that can take parsers as arguments and return a new parser as result.
The combinator is the core idea of **parser combinator**, which is an extraordinary technique 
when implmenting a parser for a specific grammar. Parsec is a parser combinator library in Haskell
and many ideas of this project came from the Parsec package.

Combinators
-----------

+ compose (`>>`)

    `pa >> pb` compose two parser into one that it first tries the first parser, if fails the the 
    combinated parser fails, else applies the second one and return is't result. In other works, 
    the result of the first parser is ignored.

+ choice (`|`)

    `pa | pb`  tries to apply the parsers to `pa` at first, if success, return the return of `pa`, 
    else try the parser `pb` with **backtracking**, and return the result of the latter.

+ joint (`+`)

    `pa + pb` joint two parsers into one and return a `std::pair` contains both results.

+ except (`-`)
    
    `pa - pb` except parser `pb` from `pa`. Only source texts that satisfies the first parser and 
    DOES NOT satisfy the second parser can be applied on successfully using the combinated parser.

+ skip (`<<`)

    `pa << pb` parse the result of parser `pa` from source text and then skip the text that satisfies
    `pb`.

+ ends_with (`<`)

    `pa < pb` parse the result of parser `pa` from source text which is ends with the text that 
    satisfies
    `pb`. Compared to `skip`, the `ends_with` combinator DOES NOT consumed the text satisfies parser
    `pb`, whether the `skip`combinator DOES consume the text.

+ times
    
    `times(p, m, n)` repeats the parser `pa` for m times at least and for n times at most, with 
    a **greedy** tactics.
    The more, the better, but under the limit of m and n.

+ optional (`~`)

    `~p` indicates that the applying the parser `p` 0 or 1 times.

+ many (`++()`)

    `many(p)` repeats a parser for zero or more times, and return a vector contains the all results.

+ many1 (`++`)

    `many1(p)` repeats a parser for zero or more times, and return a vector contains the all results.

+ countk (`*`)

    `p * k` repeats a parser for exactly `k` times, and return a vector contains the all results.

+ sepby1 (`%`)

    `sepby1 p sep` parses one or more occurrences of p, separated by sep. Returns a vector of values
    returned by p.

+ mapfn (`/`)

    `mapfn(p, fn)` applies the function `fn` to the result of parser `p`, and return the result of `fn`.

Related works
--------------

1. [The parsec package](http://hackage.haskell.org/package/parsec)
2. [Parsnip Parser Library](http://parsnip-parser.sourceforge.net/)

Reading list
------------

1. [Haskell Parsec](https://wiki.haskell.org/parsec)
2. [Combinator Parsing: A Short Tutorial](http://www.cs.uu.nl/research/techreps/repo/CS-2008/2008-044.pdf) by _S. Doaitse Swierstra_
3. [Monadic Parser Combinators](http://www.cs.nott.ac.uk/~pszgmh/monparsing.pdf) by _Graham Hutton_ and _Erik Meijer_
4, [Parsec, a fast combinator parser](http://research.microsoft.com/en-us/um/people/daan/download/parsec/parsec.pdf) by _DAAN LEIJEN_

Licence
--------

The MIT Licence, Copyright (c) 2015 He Tao
