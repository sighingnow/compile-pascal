Pl0 Compiler
============

Top-Level Error Report:
----------------------

1. Errors occurred during parsing.
2. Extra characters in source text.
3. Errors occurred during semantic analysing.


ERROR When Parsing:
------------------

1. expected character 'c'. (single symbol)
2. expected literal string "string". (keywords)
3. invalid characters to construct identifier.
4. an identifier can't be as same as anyone of the keywords.
5. expected integer or char as the primitive data type. (parse primitive type)

ERROR Recovery (skip while invalid)
---------------------

1. parse identifier

    skip all characters until get a valid identifier.

2. 

ERROR When Semantic Analysing (Type Checking):
----------------------------------------------

1. redefinition of "a"
2. use of undeclared identifier "a"
3. use an array identifier "a" as a factor.
4. expected an array identifier.
5. expected an valid expression as the array index.
6. expected an valid expression as the condition expression.
7. compare two expressions with different types.
8. do +/- operation on two factors with different types.
9. do *// operation on two terms with different types.


ABOUT TYPE CAST:
----------------

1. char can cast to integer implicitly.
2. integer can't cast to char.
3. array of char and array integer can do no type casting.
4. const value can do no type casting.



