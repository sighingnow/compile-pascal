#include <string>
#include <fstream>
#include <streambuf>

#include "pl0_parser.hpp"

input_t *load_case(char const *fname) {
    std::ifstream t(fname);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return new input_t(str);
}

int main(int argc, char **argv) {
    auto parse_tool = ParsecT<decltype(pl0_program)>(pl0_program);
    input_t *in;

    // TODO

    return 0;
}
