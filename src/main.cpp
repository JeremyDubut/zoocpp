#include <cstring>
#include <iostream>
#include <fstream>
#include "lexer.hpp"
#include "parser.tab.hh"
#include "syntax.hpp"
#include "value.hpp"
#include "rsyntax.hpp"
#include "errors.hpp"

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

        raw_ptr term;
        foo::Parser parser(lexer, debug, &term);
        int res = parser();
        std::cout << "Parsed term: " << std::endl;
        std::cout << *term << std::endl;
        std::cout << "=================" << std::endl;

        LOG("Debug log:");
        inferrance_t inf = term->infer();
        LOG("=================");
        
        std::cout << "Inferred type: " << *inf.typ->display() << std::endl;
        std::cout << "Normal form: " << *inf.term->nf()->display() << std::endl;

        return res;
    }
    catch (unification_e& e) {
        std::cout << "Unification error: " << e << std::endl;
    }
    catch (beta_red_e& e) {
        std::cout << "Beta reduction error: " << e << std::endl;
    }
    catch (reification_e& e) {
        std::cout << "Reification error: " << e << std::endl;
    }
    catch (check_e& e) {
        std::cout << "Type check error: " << e << std::endl;
    }
    catch (inferrance_e& e) {
        std::cout << "Type inferrance error: " << e << std::endl;
    }
    catch (prune_e& e) {
        std::cout << "Pruning error: " << e << std::endl;
    }
    catch (evaluation_e& e) {
        std::cout << "Evaluation error: " << e << std::endl;
    }
    catch (parser_e& e) {
        std::cout << "Parser error: " << e << std::endl;
    }
    catch (char const* e) {
        std::cout << "Error: " << e << std::endl;
    }
}