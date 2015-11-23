/**
 * Parser for PL0 grammar.
 */

#ifndef __PLO_PARSER_HPP__
#define __PLO_PARSER_HPP__

#include <tuple>
#include <algorithm>

#include "input_t.hpp"
#include "parser.hpp"
#include "combinator.hpp"

#include "pl0_ast.hpp"

extern parser_t<pl0_ast_program *> pl0_program;
extern parser_t<pl0_ast_prog *> pl0_prog;
extern parser_t<pl0_ast_const_stmt *> pl0_const_stmt;
extern parser_t<pl0_ast_const_define *> pl0_const_define;
extern parser_t<pl0_ast_constv *> pl0_const;
extern parser_t<pl0_ast_constv *> pl0_char;
extern parser_t<pl0_ast_charseq *> pl0_charseq;
extern parser_t<pl0_ast_constv *> pl0_unsigned;
extern parser_t<pl0_ast_identify *> pl0_identify;
extern parser_t<pl0_ast_var_stmt *> pl0_var_stmt;
extern parser_t<pl0_ast_var_define *> pl0_var_define;
extern parser_t<pl0_ast_type *> pl0_type;
extern parser_t<pl0_ast_primitive_type *> pl0_primitive_type;
extern parser_t<pl0_ast_executable *> pl0_executable;
extern parser_t<pl0_ast_procedure_stmt *> pl0_procedure_stmt;
extern parser_t<pl0_ast_function_stmt *> pl0_function_stmt;
extern parser_t<pl0_ast_procedure_header *> pl0_procedure_header;
extern parser_t<pl0_ast_function_header *> pl0_function_header;
extern parser_t<pl0_ast_param_list *> pl0_param_list;
extern parser_t<pl0_ast_param_group *> pl0_param_group;
extern parser_t<pl0_ast_stmt *> pl0_stmt;
extern parser_t<pl0_ast_null_stmt *> pl0_null_stmt; // empty statement, not in grammar.
extern parser_t<pl0_ast_assign_stmt *> pl0_assign_stmt;
extern parser_t<pl0_ast_function_id *> pl0_function_id;
extern parser_t<pl0_ast_expression *> pl0_expression;
extern parser_t<pl0_ast_term *> pl0_term;
extern parser_t<pl0_ast_factor *> pl0_factor;
extern parser_t<pl0_ast_call_func *> pl0_call_func;
extern parser_t<pl0_ast_arg_list *> pl0_arg_list;
extern parser_t<pl0_ast_arg *> pl0_arg;
extern parser_t<pl0_ast_addop *> pl0_addop;
extern parser_t<pl0_ast_multop *> pl0_multop;
extern parser_t<pl0_ast_condtion *> pl0_condition;
extern parser_t<pl0_ast_comp_op *> pl0_comp_op;
extern parser_t<pl0_ast_cond_stmt *> pl0_cond_stmt;
extern parser_t<pl0_ast_case_stmt *> pl0_case_stmt;
extern parser_t<pl0_ast_case_term *> pl0_case_term;
extern parser_t<pl0_ast_for_stmt *> pl0_for_stmt;
extern parser_t<pl0_ast_call_proc *> pl0_call_proc;
extern parser_t<pl0_ast_compound_stmt *> pl0_compound_stmt;
extern parser_t<pl0_ast_read_stmt *> pl0_read_stmt;
extern parser_t<pl0_ast_write_stmt *> pl0_write_stmt;
extern parser_t<pl0_ast_alnum *> pl0_alpha;
extern parser_t<pl0_ast_alnum *> pl0_digit;

using namespace std;

void pl0_parser_error(pair<int, int> loc, string msg) {
    // status = false;
    cout << loc.first << ":" << loc.second << " " << "ERRRO: " << msg << endl;
}

static function<string (vector<char>)> vec2str = [](const vector<char> & vec) {
    string ans; for (char c: vec) { ans.push_back(c); } return ans;
};
static const auto spaces = (++space)/vec2str;

static bool verbose = true;

// porting the `character` and `string_literal` parser generator from libparsec, for reporting parse error.
pair<int, char> pl0_character_helper(char const & ch, input_t *text) {
    if (text->empty()) { return make_pair(-1, '\0'); }
    char c = text->at(0);
    if (ch != c) { pl0_parser_error(text->locate(), string("expected character ") + "'" + string(0, ch) + "'"); }
    return make_pair(ch == c ? (1) : (-1), ch);
}
parser_t<char> pl0_character(char const & ch) {
    auto fn = std::bind(character_helper, ch, std::placeholders::_1);
    return parser_t<char>("character: " + string(1, ch), fn);
}
pair<int, string> pl0_string_literal_helper(const string & str, input_t *text) {
    if (text->empty()) { return make_pair(-1, "\0"); }
    string s = text->take(str.length());
    if (str != s) { pl0_parser_error(text->locate(), string("expected literal string ") + "\"" + str + "\""); }
    return make_pair(str == s ? str.length() : (-1), str);
}
parser_t<string> pl0_string_literal(string const & str) {
    auto fn = std::bind(string_literal_helper, str, std::placeholders::_1);
    return parser_t<string>("string literal: " + str, fn);
}

// <程序> ::= <分程序>.
pair<int, pl0_ast_program *> pl0_program_fn(input_t *text) {
    auto parser = pl0_prog << pl0_character('.');
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Program" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_program(text->locate(), std::get<1>(res)));
}

// <分程序> ::= [<常量说明部分>][<变量说明部分>]{[<过程说明部分>]| [<函数说明部分>]}<复合语句>
pair<int, pl0_ast_prog *> pl0_prog_fn(input_t *text) {
    auto parser = ((~pl0_const_stmt) + (~pl0_var_stmt)) + (++pl0_executable + pl0_compound_stmt);
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Prog" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_prog(
        text->locate(), 
        std::get<1>(res).first.first,
        std::get<1>(res).first.second,
        std::get<1>(res).second.first,
        std::get<1>(res).second.second
    ));
}

// <数字> ::= 0|1|2|3 ... 8|9
pair<int, pl0_ast_alnum *> pl0_digit_fn(input_t *text) {
    auto res = digit(text);
    // if (verbose) {
    //     cout << "parsing: Digit" << endl;
    // }
    return make_pair(std::get<0>(res), new pl0_ast_alnum(text->locate(), std::get<1>(res)));
}

// <字母> ::= a|b|c|d ... x|y|z |A|B…|Z
pair<int, pl0_ast_alnum *> pl0_alpha_fn(input_t *text) {
    auto res = alpha(text);
    // if (verbose) {
    //     cout << "parsing: Alpha" << endl;
    // }
    return make_pair(std::get<0>(res), new pl0_ast_alnum(text->locate(), std::get<1>(res)));
}

// <字符> ::= '<字母>' | '<数字>'
pair<int, pl0_ast_constv *> pl0_char_fn(input_t *text) {
    auto parser = pl0_character('\'') >> (pl0_digit | pl0_alpha) << pl0_character('\'');
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Char" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_constv(text->locate(), std::get<1>(res) ? std::get<1>(res)->val : 0));
}

// <字符串> ::= "{十进制编码为32,33,35-126的ASCII字符}"
pair<int, pl0_ast_charseq *> pl0_charseq_fn(input_t *text) {
    auto parser = pl0_character('"') >> (++(print-pl0_character('"'))) << pl0_character('"');
    auto res = (spaces >> (parser / vec2str) << spaces)(text);
    if (verbose) {
        cout << "parsing: Charseq" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_charseq(text->locate(), std::get<1>(res)));
}

// <无符号整数> ::= <数字>{<数字>}
pair<int, pl0_ast_constv *> pl0_unsigned_fn(input_t *text) {
    function<unsigned int (vector<pl0_ast_alnum *>)> fn = [](vector<pl0_ast_alnum *> vec) {
        unsigned int ans = 0; for (auto k: vec) { ans = ans * 10 + (k ? k->val - '0' : 0); } return ans;
    };
    auto res = ((spaces >> (pl0_digit++) << spaces) / fn)(text);
    if (verbose) {
        cout << "parsing: Unsigned" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_constv(text->locate(), std::get<1>(res)));
}

// <常量> ::= [+|-]<无符号整数>|<字符>
pair<int, pl0_ast_constv *> pl0_const_fn(input_t *text) {
    auto res1 = (spaces >> (pl0_character('+') | pl0_character('-')) << spaces)(text);
    pair<int, pl0_ast_constv *> ans;
    if (std::get<0>(res1) != -1) {
        int flag = std::get<1>(res1) == '+' ? (1) : (-1);
        auto res2 = pl0_unsigned(text->drop(std::get<0>(res1)));
        ans = make_pair(
            std::get<0>(res2) == -1 ? -1 : (std::get<0>(res1) + std::get<0>(res2)),
            new pl0_ast_constv(text->locate(), std::get<1>(res2)->val * flag)
        );
    }
    else {
        auto res = (pl0_unsigned | pl0_char)(text);
        ans = make_pair(std::get<0>(res), new pl0_ast_constv(text->locate(), std::get<1>(res) ? std::get<1>(res)->val : 0));
    }
    if (verbose) {
        cout << "parsing: Constant" << endl;
    }
    return ans;
}

// <标识符> ::= <字母>{<字母>|<数字>}
pair<int, pl0_ast_identify *> pl0_identify_fn(input_t *text) {
    static std::array<string, 17> keywords = {{
        "begin", "end", "for", "downto", "to", "do", "if", "then", "else", "case",
        "const", "var", "array", "of", "case", "integer", "char"}};
    function<string (pair<pl0_ast_alnum *, vector<pl0_ast_alnum *>>)> fn = [](pair<pl0_ast_alnum *, vector<pl0_ast_alnum *>> const & p) {
        string ans = string(1, p.first ? p.first->val : 0); for (pl0_ast_alnum *c: p.second) { ans.push_back(c ? c->val : 0); } return ans;
    };
    auto parser = pl0_alpha + (++(pl0_alpha | pl0_digit));
    auto res = (spaces >> (parser / fn) << spaces)(text);
    pair<int, pl0_ast_identify *> ans;
    if (std::get<0>(res) != -1 && std::none_of(keywords.begin(), keywords.end(), [&, res](string s) { return s == std::get<1>(res); })) {
        ans = make_pair(std::get<0>(res), new pl0_ast_identify(text->locate(), std::get<1>(res)));
    }
    else {
        ans = make_pair(-1, new pl0_ast_identify(text->locate(), ""));
    }
    if (verbose) {
        cout << "parsing: Identifier" << endl;
    }
    return ans;
}

// <基本类型> ::= integer | char
pair<int, pl0_ast_primitive_type *> pl0_primitive_type_fn(input_t *text) {
    auto parser = pl0_string_literal("integer") | pl0_string_literal("char");
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Primitive Type" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_primitive_type(text->locate(), std::get<1>(res)));
}

// <类型> ::= <基本类型>|array'['<无符号整数>']' of <基本类型>
pair<int, pl0_ast_type *> pl0_type_fn(input_t *text) {
    function<pair<pl0_ast_constv *, pl0_ast_primitive_type *> (pl0_ast_primitive_type *)> fn = [&, text](pl0_ast_primitive_type *t) { return make_pair(new pl0_ast_constv(text->locate(), -1), t); };
    auto parser = (pl0_primitive_type / fn) // not array
        | ((pl0_string_literal("array") >> spaces >> (pl0_character('[') >> pl0_unsigned << pl0_character(']')) << spaces << pl0_string_literal("of")) + pl0_primitive_type); // array
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Type" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_type(text->locate(), std::get<1>(res).second, std::get<1>(res).first ? std::get<1>(res).first->val : -1));
}

// <加法运算符> ::= +|-
pair<int, pl0_ast_addop *> pl0_addop_fn(input_t *text) {
    auto res = (spaces >> (pl0_character('+') | pl0_character('-')) << spaces)(text);
    if (verbose) {
        cout << "parsing: Add/Minus" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_addop(text->locate(), std::get<1>(res)));
}

// <乘法运算符> ::= *|/
pair<int, pl0_ast_multop *> pl0_multop_fn(input_t *text) {
    auto res = (spaces >> (pl0_character('*') | pl0_character('/')) << spaces)(text);
    if (verbose) {
        cout << "parsing: Mult/Div" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_multop(text->locate(), std::get<1>(res)));
}

// <变量说明> ::= <标识符>{, <标识符>} : <类型>
pair<int, pl0_ast_var_define *> pl0_var_define_fn(input_t *text) {
    auto parser = (pl0_identify % pl0_character(',')) + (pl0_character(':') >> pl0_type);
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Variable Define" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_var_define(text->locate(), std::get<1>(res).second, std::get<1>(res).first));
}

// <变量说明部分> ::= var <变量说明> ; {<变量说明>;}
pair<int, pl0_ast_var_stmt *> pl0_var_stmt_fn(input_t *text) {
    auto parser = pl0_string_literal("var") >> ((pl0_var_define << pl0_character(';'))++);
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Variable Statement" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_var_stmt(text->locate(), std::get<1>(res)));
}

// <常量定义> ::= <标识符>＝ <常量>
pair<int, pl0_ast_const_define *> pl0_const_define_fn(input_t *text) {
    auto parser = pl0_identify + (pl0_character('=') >> pl0_const);
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Constant Define" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_const_define(text->locate(), std::get<1>(res).first, std::get<1>(res).second));
}

// <常量说明部分> ::= const<常量定义>{,<常量定义>};
pair<int, pl0_ast_const_stmt *> pl0_const_stmt_fn(input_t *text) {
    auto parser = pl0_string_literal("const") >> (pl0_const_define % pl0_character(',')) << pl0_character(';');
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Constant Statement" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_const_stmt(text->locate(), std::get<1>(res)));
}

// <形式参数段> ::= [var]<标识符>{, <标识符>}: <基本类型>
pair<int, pl0_ast_param_group *> pl0_param_group_fn(input_t *text) {
    auto parser = ((spaces >> (~pl0_string_literal("var")) << spaces) >> (pl0_identify % pl0_character(','))) + (pl0_character(':') >> pl0_primitive_type);
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Parameter Group" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_param_group(text->locate(), std::get<1>(res).first, std::get<1>(res).second));
}

// <形式参数表> ::= '(' <形式参数段>{; <形式参数段>}')'
pair<int, pl0_ast_param_list *> pl0_param_list_fn(input_t *text) {
    auto parser = pl0_character('(') >> (pl0_param_group % pl0_character(';')) << pl0_character(')');
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Parameter List" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_param_list(text->locate(), std::get<1>(res)));
}

// <函数标识符> ::= <标识符>
pair<int, pl0_ast_function_id *> pl0_function_id_fn(input_t *text) {
    auto res = pl0_identify(text);
    if (verbose) {
        cout << "parsing: Function Identifier" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_function_id(text->locate(), std::get<1>(res)));
}

// <过程首部> ::= procedure<标识符>[<形式参数表>];
pair<int, pl0_ast_procedure_header *> pl0_procedure_header_fn(input_t *text) {
    auto parser = pl0_string_literal("procedure") >> (pl0_identify + (~pl0_param_list)) << pl0_character(';');
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Procedure Header" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_procedure_header(text->locate(), std::get<1>(res).first, std::get<1>(res).second));
}

// <函数首部> ::= function <标识符>[<形式参数表>]: <基本类型>;
pair<int, pl0_ast_function_header *> pl0_function_header_fn(input_t *text) {
    auto parser = (pl0_string_literal("function") >> (pl0_identify + (~pl0_param_list))) + (pl0_character(':') >> pl0_primitive_type << pl0_character(';'));
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Function Header" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_function_header(text->locate(), std::get<1>(res).first.first, std::get<1>(res).first.second, std::get<1>(res).second));
}

// <过程说明部分> ::= <过程首部><分程序>{; <过程首部><分程序>};
pair<int, pl0_ast_procedure_stmt *> pl0_procedure_stmt_fn(input_t *text) {
    auto res = (((pl0_procedure_header + pl0_prog) << pl0_character(';'))++)(text);
    if (verbose) {
        cout << "parsing: Procedure Statement" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_procedure_stmt(text->locate(), std::get<1>(res)));
}

// <函数说明部分> ::= <函数首部><分程序>{; <函数首部><分程序>};
pair<int, pl0_ast_function_stmt *> pl0_function_stmt_fn(input_t *text) {
    auto res = (((pl0_function_header + pl0_prog) << pl0_character(';'))++)(text);
    if (verbose) {
        cout << "parsing: Function Statement" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_function_stmt(text->locate(), std::get<1>(res)));
}

// 可调用对象 ::= <函数说明部分> | <过程说明部分>
pair<int, pl0_ast_executable *> pl0_executable_fn(input_t *text) {
    auto res = pl0_function_header(text);
    pair<int, pl0_ast_executable *> ans;
    if (std::get<0>(res) != -1) {
        auto res1 = pl0_function_stmt(text);
        ans = make_pair(std::get<0>(res1), new pl0_ast_executable(text->locate(), std::get<1>(res1)));
    }
    else {
        auto res2 = pl0_procedure_stmt(text);
        ans = make_pair(std::get<0>(res2), new pl0_ast_executable(text->locate(), std::get<1>(res2)));
    }
    if (verbose) {
        cout << "parsing: Executable (Function and Procedure)" << endl;
    }
    return ans;
}

// <空语句>
pair<int, pl0_ast_null_stmt *> pl0_null_stmt_fn(input_t *text) {
    if (verbose) {
        cout << "parsing: Null Statement" << endl;
    }
    return make_pair(0, new pl0_ast_null_stmt(text->locate()));
}

// <赋值语句> ::= <标识符> := <表达式>| <函数标识符> := <表达式> | <标识符>'['<表达式>']':= <表达式>
pair<int, pl0_ast_assign_stmt *> pl0_assign_stmt_fn(input_t *text) {
    auto parser = pl0_identify + (~(pl0_character('[') >> pl0_expression << pl0_character(']'))) + (spaces >> pl0_string_literal(":=") >> pl0_expression);
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Assign Statement" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_assign_stmt(
        text->locate(),
        std::get<1>(res).first.first,
        std::get<1>(res).first.second,
        std::get<1>(res).second
    ));
}

// <语句> ::= <赋值语句>|<条件语句>|<情况语句>|<过程调用语句>|<复合语句>|<读语句>|<写语句>|<for循环语句>|<空>
pair<int, pl0_ast_stmt *> pl0_stmt_fn(input_t *text) {
    pair<int, pl0_ast_stmt *> ans;
    auto res1 = pl0_write_stmt(text);
    if (std::get<0>(res1) != -1) {
        ans = make_pair(std::get<0>(res1), std::get<1>(res1));
    }
    else {
        auto res2 = pl0_read_stmt(text);
        if (std::get<0>(res2) != -1) {
            ans = make_pair(std::get<0>(res2), std::get<1>(res2));
        }
        else {
            auto res3 = pl0_assign_stmt(text);
            if (std::get<0>(res3) != -1) {
                ans = make_pair(std::get<0>(res3), std::get<1>(res3));
            }
            else {
                auto res4 = pl0_cond_stmt(text);
                if (std::get<0>(res4) != -1) {
                    ans = make_pair(std::get<0>(res4), std::get<1>(res4));
                }
                else {
                    auto res5 = pl0_case_stmt(text);
                    if (std::get<0>(res5) != -1) {
                        ans = make_pair(std::get<0>(res5), std::get<1>(res5));
                    }
                    else {
                        auto res6 = pl0_compound_stmt(text);
                        if (std::get<0>(res6) != -1) {
                            ans = make_pair(std::get<0>(res6), std::get<1>(res6));
                        }
                        else {
                            auto res7 = pl0_for_stmt(text);
                            if (std::get<0>(res7) != -1) {
                                ans = make_pair(std::get<0>(res7), std::get<1>(res7));
                            }
                            else {
                                auto res8 = pl0_call_proc(text);
                                if (std::get<0>(res8) != -1) {
                                    ans = make_pair(std::get<0>(res8), std::get<1>(res8));
                                }
                                else {
                                    // default: empty statement.
                                    ans = make_pair(0, new pl0_ast_null_stmt(text->locate()));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (verbose) {
        cout << "parsing: Statement" << endl;
    }
    return ans;
}

// <表达式> ::= [+|]<项>{<加法运算符><项>}
pair<int, pl0_ast_expression *> pl0_expression_fn(input_t *text) {
    auto res1 = ((~pl0_addop) + pl0_term)(text);
    pair<int, pl0_ast_expression *> ans;
    if (std::get<0>(res1) == -1) {
        auto vec = std::vector<std::pair<struct pl0_ast_addop *, struct pl0_ast_term *>>();
        ans = make_pair(-1, new pl0_ast_expression(text->locate(), vec));
    }
    else {
        std::pair<struct pl0_ast_addop *, struct pl0_ast_term *> head;
        int headlen = std::get<0>(res1);
        if (std::get<1>(res1).first == nullptr) {
            head = make_pair(new pl0_ast_addop(text->locate(), '+'), std::get<1>(res1).second); // default: '+'
        }
        else {
            head = std::get<1>(res1);
        }
        auto res2 = (++(pl0_addop + pl0_term))(text->drop(headlen));
        auto vec = std::get<1>(res2);
        vec.emplace(vec.begin(), head);
        ans = make_pair(headlen + std::get<0>(res2), new pl0_ast_expression(text->locate(), vec));
    }
    if (verbose) {
        cout << "parsing: Expression" << endl;
    }
    return ans;
}

// <实在参数> ::= <表达式>
pair<int, pl0_ast_arg *> pl0_arg_fn(input_t *text) {
    auto res = pl0_expression(text);
    if (verbose) {
        cout << "parsing: Argument" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_arg(text->locate(), std::get<1>(res)));
}

// <实在参数表> ::= '('<实在参数> {, <实在参数>}')'
pair<int, pl0_ast_arg_list *> pl0_arg_list_fn(input_t *text) {
    auto res = (pl0_character('(') >> (pl0_arg % pl0_character(',')) << pl0_character(')'))(text);
    if (verbose) {
        cout << "parsing: Argument List" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_arg_list(text->locate(), std::get<1>(res)));
}

// <函数调用语句> ::= <标识符>[<实在参数表>]
pair<int, pl0_ast_call_func *> pl0_call_func_fn(input_t *text) {
    auto parser = pl0_identify + (~pl0_arg_list);
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Function Call" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_call_func(text->locate(), std::get<1>(res)));
}

// <项> ::= <因子>{<乘法运算符><因子>}
pair<int, pl0_ast_term *> pl0_term_fn(input_t *text) {
    auto parser = pl0_factor >= pl0_multop;
    auto res = parser(text);
    if (verbose) {
        cout << "parsing: Term" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_term(text->locate(), std::get<1>(res)));
}

// <因子> ::= <标识符> | <无符号整数> |'('<表达式>')' | <函数调用语句> | <标识符>'['<表达式>']'
pair<int, pl0_ast_factor *> pl0_factor_fn(input_t *text) {
    auto res1 = (pl0_identify + (pl0_character('[') >> pl0_expression << pl0_character(']')))(text);
    pair<int, pl0_ast_factor *> ans;
    if (std::get<0>(res1) != -1) {
        ans = make_pair(std::get<0>(res1), new pl0_ast_factor(text->locate(), std::get<1>(res1)));
    }
    else {
        auto res2 = (pl0_character('(') >> pl0_expression << pl0_character(')'))(text);
        if (std::get<0>(res2) != -1) {
            ans = make_pair(std::get<0>(res2), new pl0_ast_factor(text->locate(), pl0_ast_factor::type_t::EXPR, std::get<1>(res2)));
        }
        else {
            auto res3 = pl0_unsigned(text);
            if (std::get<0>(res3) != -1) {
                ans = make_pair(std::get<0>(res3), new pl0_ast_factor(text->locate(), pl0_ast_factor::type_t::UNSIGNED, std::get<1>(res3)));
            }
            else {
                auto res4 = pl0_call_func(text);
                if (std::get<0>(res4) != -1) {
                    ans = make_pair(std::get<0>(res4), new pl0_ast_factor(text->locate(), pl0_ast_factor::type_t::CALL_FUNC, std::get<1>(res4)));
                }
                else {
                    auto res5 = pl0_identify(text);
                    ans = make_pair(std::get<0>(res5), new pl0_ast_factor(text->locate(), pl0_ast_factor::type_t::ID, std::get<1>(res5)));
                }
            }
        }
    }
    if (verbose) {
        cout << "parsing: Factor" << endl;
    }
    return ans;
}

// <关系运算符> ::= <|<=|>|>= |=|<>
pair<int, pl0_ast_comp_op *> pl0_comp_op_fn(input_t *text) {
    auto parser = pl0_string_literal("<=")
        | pl0_string_literal("<>")
        | pl0_string_literal(">=")
        | pl0_string_literal("<")
        | pl0_string_literal(">")
        | pl0_string_literal("=");
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Compare Operator(LE, NE, GE, LT, GT, EQ)" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_comp_op(text->locate(), std::get<1>(res)));
}

// <条件> ::= <表达式><关系运算符><表达式>
pair<int, pl0_ast_condtion *> pl0_condition_fn(input_t *text) {
    auto res = (spaces >> (pl0_expression + pl0_comp_op + pl0_expression) << spaces)(text);
    if (verbose) {
        cout << "parsing: Condition Expression" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_condtion(
        text->locate(),
        std::get<1>(res).first.first,
        std::get<1>(res).first.second,
        std::get<1>(res).second
    ));
}

// <条件语句> ::= if<条件>then<语句> | if<条件>then<语句>else<语句>
// **** 增加限制：else 后面的 <语句> 不能为 <空> ****
pair<int, pl0_ast_cond_stmt *> pl0_cond_stmt_fn(input_t *text) {
    auto parser = (pl0_string_literal("if") >> pl0_condition) + (pl0_string_literal("then") >> pl0_stmt)
        + ~(pl0_string_literal("else") >> pl0_stmt);
    auto res = (spaces >> parser << spaces)(text);
    pair<int, pl0_ast_cond_stmt *> ans;
    if (std::get<1>(res).second && std::get<1>(res).second->t == pl0_ast_stmt::type_t::NULL_STMT) {
        ans = make_pair(-1, new pl0_ast_cond_stmt(text->locate(), nullptr, nullptr, nullptr));
    }
    else { 
        ans = make_pair(std::get<0>(res), new pl0_ast_cond_stmt(
            text->locate(),
            std::get<1>(res).first.first,
            std::get<1>(res).first.second,
            std::get<1>(res).second
        ));
    }
    if (verbose) {
        cout << "parsing: Condition Statement" << endl;
    }
    return ans;
}

// <情况语句> ::= case <表达式> of <情况表元素>{; <情况表元素>} end
pair<int, pl0_ast_case_stmt *> pl0_case_stmt_fn(input_t *text) {
    auto parser = (pl0_string_literal("case") >> pl0_expression << pl0_string_literal("of"))
        + ((pl0_case_term % pl0_character(';')) << (spaces >> pl0_string_literal("end")));
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Case Statement" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_case_stmt(text->locate(), std::get<1>(res).first, std::get<1>(res).second));
}

// <情况表元素> ::= <常量> : <语句>
pair<int, pl0_ast_case_term *> pl0_case_term_fn(input_t *text) {
    auto res = ((pl0_const << pl0_character(':')) + pl0_stmt)(text);
    if (verbose) {
        cout << "parsing: Cast Item" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_case_term(text->locate(), std::get<1>(res).first, std::get<1>(res).second));
}

// <for循环语句> ::= for <标识符> := <表达式> (downto | to) <表达式> do <语句> // 步长为1
// **** 增加限制：do 后面的 <语句> 不能为 <空> ****
pair<int, pl0_ast_for_stmt *> pl0_for_stmt_fn(input_t *text) {
    auto parser = ((pl0_string_literal("for") >> pl0_identify) + (pl0_string_literal(":=") >> pl0_expression))
        + (spaces >> (pl0_string_literal("downto") | pl0_string_literal("to")) << spaces)
        + (pl0_expression + (pl0_string_literal("do") >> pl0_stmt));
    auto res = (spaces >> parser << spaces)(text);
    pair<int, pl0_ast_for_stmt *> ans;
    if (std::get<1>(res).second.second && std::get<1>(res).second.second->t == pl0_ast_stmt::type_t::NULL_STMT) {
        ans = make_pair(-1, new pl0_ast_for_stmt(text->locate(), nullptr, nullptr, nullptr, nullptr, nullptr));
    }
    else {
        ans = make_pair(std::get<0>(res), new pl0_ast_for_stmt(
            text->locate(),
            std::get<1>(res).first.first.first,
            std::get<1>(res).first.first.second,
            std::get<1>(res).second.first,
            std::get<1>(res).second.second,
            std::get<1>(res).first.second == "downto" ? new pl0_ast_constv(text->locate(), -1) : new pl0_ast_constv(text->locate(), 1)
        ));
    }
    if (verbose) {
        cout << "parsing: For Loop Statement" << endl;
    }
    return ans;
}

// <过程调用语句> ::= <标识符>[<实在参数表>]
pair<int, pl0_ast_call_proc *> pl0_call_proc_fn(input_t *text) {
    auto parser = pl0_identify + (~pl0_arg_list);
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Procedure Call" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_call_proc(text->locate(), std::get<1>(res).first, std::get<1>(res).second));
}

// <复合语句> ::= begin<语句>{; <语句>}end
pair<int, pl0_ast_compound_stmt *> pl0_compound_stmt_fn(input_t *text) {
    auto parser = pl0_string_literal("begin") >> (pl0_stmt % (spaces >> pl0_character(';') << spaces)) << spaces << pl0_string_literal("end");
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Compound Statement" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_compound_stmt(text->locate(), std::get<1>(res)));
}

// <读语句> ::= read'('<标识符>{,<标识符>}')'
pair<int, pl0_ast_read_stmt *> pl0_read_stmt_fn(input_t *text) {
    auto parser = pl0_string_literal("read") >> spaces >> pl0_character('(') >> (pl0_identify % pl0_character(',')) << pl0_character(')');
    auto res = (spaces >> parser << spaces)(text);
    if (verbose) {
        cout << "parsing: Read Statement" << endl;
    }
    return make_pair(std::get<0>(res), new pl0_ast_read_stmt(text->locate(), std::get<1>(res)));
}

// <写语句> ::= write '(' <字符串>,<表达式> ')'|write'(' <字符串> ')'|write'('<表达式>')'
pair<int, pl0_ast_write_stmt *> pl0_write_stmt_fn(input_t *text) {
    pair<int, pl0_ast_write_stmt *> ans;
    auto parser1 = pl0_charseq + (pl0_character(',') >> pl0_expression);
    auto parser2 = pl0_charseq;
    auto parser3 = pl0_expression;
    auto res1 = (spaces >> (pl0_string_literal("write") >> spaces >> pl0_character('(') >> parser1 << pl0_character(')')) << spaces)(text);
    if (std::get<0>(res1) != -1) {
        ans = make_pair(std::get<0>(res1), new pl0_ast_write_stmt(text->locate(), std::get<1>(res1).first, std::get<1>(res1).second));
    }
    else {
        auto res2 = (spaces >> (pl0_string_literal("write") >> spaces >> pl0_character('(') >> parser2 << pl0_character(')')) << spaces)(text);
        if (std::get<0>(res2) != -1) {
            ans = make_pair(std::get<0>(res2), new pl0_ast_write_stmt(text->locate(), std::get<1>(res2)));
        }
        else {
            auto res3 = (spaces >> (pl0_string_literal("write") >> spaces >> pl0_character('(') >> parser3 << pl0_character(')')) << spaces)(text);
            ans = make_pair(std::get<0>(res3), new pl0_ast_write_stmt(text->locate(), std::get<1>(res3)));
        }
    }
    if (verbose) {
        cout << "parsing: Write Statement" << endl;
    }
    return ans;
}

#endif /* __PLO_PARSER_HPP__ */


