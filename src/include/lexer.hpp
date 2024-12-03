#pragma once

#include <string>
#if ! defined(yyFlexLexerOnce)
#define yyFlexLexer yy_foo_FlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif
#include "location_t.hpp"
#include "rsyntax.hpp"

namespace foo
{
    class Lexer : public yy_foo_FlexLexer
    {
        std::size_t currentLine = 1;
        
        std::shared_ptr<raw_t> *yylval = nullptr;
        location_t *yylloc = nullptr;
        
        void copyValue(const std::size_t leftTrim = 0, const std::size_t rightTrim = 0, const bool trimCr = false);
        void copyLocation() { *yylloc = location_t(currentLine, currentLine); }
        
    public:
        Lexer(std::istream &in, const bool debug) : yy_foo_FlexLexer(&in) { yy_foo_FlexLexer::set_debug(debug); }
        std::shared_ptr<raw_t> value();
        
        using yy_foo_FlexLexer::yylex;
        int yylex(std::shared_ptr<raw_t>* lval, location_t *const lloc);
    };
    
    inline std::shared_ptr<raw_t> Lexer::value() {return *yylval;}
    inline void Lexer::copyValue(const std::size_t leftTrim, const std::size_t rightTrim, const bool trimCr)
    {
        std::size_t endPos = yyleng - rightTrim;
        if (trimCr && endPos != 0 && yytext[endPos - 1] == '\r')
            --endPos;
        *yylval = std::make_shared<rvar_t>(std::string(yytext + leftTrim, yytext + endPos));
    }
}
