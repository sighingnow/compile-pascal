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


ERROR When Semantic Analysing (Type Checking):
----------------------------------------------

1. redefinition of "a"
2. use of undeclared identifier "a"
3. use an array identifier "a" as a factor.
4. expected an array identifier.
5. expected an non-array identifier.
6. expected an valid expression as the array index.
7. expected an valid expression as the condition expression.
8. compare two expressions with different types.
9. do +/- operation on two factors with different types.
10. do *// operation on two terms with different types.
11. duplicate identifier "F" (when there is a variable with the same name of it's container function or procedure)
12. overloaded identifier "F" isn't a function (when there is a function or procedure with the same name of it's container function or procedure, or define a function or procedure when there already is a variable or constant with the same name.)

Error: Incompatible types: got "SmallInt" expected "Char"  

ABOUT TYPE CAST:
----------------

1. char can cast to integer implicitly.
2. integer can't cast to char.
3. array of char and array integer can do no type casting.
4. const value can do no type casting.

ABOUT x86 Code Generate

1. 寄存器分配
2. 寻址：根据深度，从ebp逐级向上找。
3. 在对引用类型的变量分配寄存器时，不能分配 eax 寄存器。（eax寄存器要用于引用变量的寻址过程）


一个关于中间代码的问题：先将函数嵌套拆分(flatten)

对于for循环的汇编代码生成：在循环开始处和循环结束处都将循环变量加载至 eax 寄存器中。（认为循环变量是活跃的）

调用C库函数scanf和printf之前需要保存eax, ebx, ecx寄存器。（eax会被用作这两个函数的返回值寄存器）
**遵守x86 ABI的调用约定**

对于分支，如果一个分支内有临时变量溢出到栈上，另一个分支内没有，栈的平衡就会被破坏。一个分支内产生的临时变量无法到达分支之后的基本块中。

四元式：

1. = target source
2. []= array index source
3. =[] target array index
5. loadret
6. call func/proc {args} [return]
11. read target
12. write_s string label
13. write_e value

14. + c a b
15. - c a b
16. * c a b
17. / c a b
18. % c a b
19. cmp a b
20. label label_no
21. goto label_no
22. def // 定义循环语句的结束处的临时变量变量和case语句的条件表达式临时变量，在划分基本块时，def 语句被整合到函数或者过程头部。

优化：

1. 划分基本块：在四元式中，每个基本块对应一个label，用label编号作为基本块的编号。对于函数头声明，该块编号为 0
2. 将函数调用也作为划分基本块的边界依据。
** 有两种临时变量的作用范围会跨越基本块：

+ for 循环语句，作为结束条件的临时变量
+ case 语句，作为condition的临时变量。

3. 按照pascal的语义，循环语句中，无论循环体是否执行，执行多少次，在循环结束后，循环变量的值都等于 end 的值。（解决方案：在循环末尾生成显式赋值语句）