#include <string>
#include <fstream>
#include <streambuf>

#include "input_t.hpp"
#include "parse_tool.hpp"

#include "pl0_ast.hpp"

using namespace std;

extern parser_t<pl0_ast_program *> pl0_program;
extern struct IRBuilder irb;
bool pl0_tac_program(pl0_ast_program const *program);

bool verbose = true;

input_t *load_case(char const *fname) {
    std::ifstream t(fname);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return new input_t(str);
}

int main(int argc, char **argv) {
    auto parse_tool = ParsecT<decltype(pl0_program)>(pl0_program);
    input_t *in = load_case(argv[1]);
    auto res = parse_tool(in);
    if (!res.status) {
        cout << "Errors occurred during parsing." << endl;
        return 0;
    }
    if (!res.strict) {
        cout << "Extra characters in source text." << endl;
    }
    if (!pl0_tac_program(res.actual)) {
        cout << "Errors occurred during semantic analysing." << endl;
        // return 0;
    }
    irb.dump();

    return 0;
}


