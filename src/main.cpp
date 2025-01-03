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

        #ifdef DEBUG
            LOG("Debug log:");
        #endif
        inferrance_t inf = term->infer();
        #ifdef DEBUG
            LOG("=================");
        #endif
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
    catch (char const* e) {
        std::cout << "Error: " << e << std::endl;
    }
    catch (std::string e) {
        std::cout << "Error: " << e << std::endl;
        // std::cout << "Number of postponed checks: " << check_t::lookupTable.size() << std::endl;
        std::size_t count = 0;
        for (check_ptr it : check_t::lookupTable) {
            std::cout << "Check number " << count << " " << *it << std::endl;
            count++;
        }
    }
    catch (std::out_of_range const& e) {
        std::cout << "Out of range: " << e.what() << std::endl;
    }
}