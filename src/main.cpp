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
    try {
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
        std::cout << "Parsed term: " << std::endl;
        std::cout << *term << std::endl;
        std::cout << "=================" << std::endl;

        context_t ctx = context_t();
        environment_t env = environment_t();
        #ifdef DEBUG
            LOG("Debug log:");
        #endif
        inferrance_t inf = term->infer(ctx);
        #ifdef DEBUG
            LOG("=================");
        #endif
        std::cout << "Inferred type: " << *inf.typ->display() << std::endl;
        std::cout << "Normal form: " << *inf.term->nf(env)->display() << std::endl;

        return res;
    }
    catch (char const* e) {
        std::cout << "Error: " << e << std::endl;
    }
    catch (std::string e) {
        std::cout << "Error: " << e << std::endl;
    }
    catch (std::out_of_range const& e) {
        std::cout << "Out of range: " << e.what() << std::endl;
    }
}