#include <cstring>
#include <iostream>
#include <fstream>
#include "lexer.hpp"
#include "parser.tab.hh"
#include "syntax.hpp"
#include "value.hpp"
#include "rsyntax.hpp"

int main(int argc, char* argv[])
{
    const bool debug = argc > 1 && std::strcmp(argv[1], "-d") == 0;
    const char* nameFile;
    const bool withFile = (argc > 1 && std::strcmp(argv[1], "-f") == 0) ||
        (argc > 2 && std::strcmp(argv[2], "-f") == 0);
    std::istream* in = &std::cin;
    std::ifstream fb;
    if (withFile) {
        if (argc > 3) {
            nameFile = argv[3];
        }
        else {
            nameFile = argv[2];
        }
        fb.open (nameFile);
        if (fb.is_open()) {
            in = &fb;
        }
        else {
            std::cout << "File not open" << std::endl;
            return 1;
        }
    }
    foo::Lexer lexer(*in, debug);

    std::shared_ptr<raw_t> term;
    foo::Parser parser(lexer, debug, &term);
    int res = parser();

    std::cout << "Parsed term: " << std::endl << *term << std::endl;

    // environment_t env;

    // // std::shared_ptr<value_t> value = term->eval(env);

    // // // std::cout << "Weak head normal form: " << std::endl;
    // // // std::cout << *value << std::endl;

    // env = environment_t();

    // std::shared_ptr<term_t> nf = term->nf(env);
    // std::cout << "Normal form: " << std::endl;
    // std::cout << *nf << std::endl;

    return res;
}