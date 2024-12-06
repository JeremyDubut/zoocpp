%{
    #include <iostream>
    #include "lexer.hpp"
    #include "parser.tab.hh"
    #include "common.hpp"
    
    using namespace foo;
    
    #undef  YY_DECL
    #define YY_DECL int Lexer::yylex(raw_ptr* lval, location_t *const lloc)
    
    #define YY_USER_INIT yylval = lval; yylloc = lloc;
    
    #define YY_USER_ACTION copyLocation();
%}

%option c++ noyywrap debug

%option yyclass="Lexer"
%option prefix="yy_foo_"

%%

%{
    using Token = Parser::token;
%}

"\n" { ++currentLine; return Token::NL; }
[[:space:]] ;
"_" { return Token::HOLE; }
"λ" { return Token::LAMBDA; }
"\\" { return Token::LAMBDA; }
"let" { return Token::LET; }
":" { return Token::COLON; }
"=" { return Token::EQ; }
"->" { return Token::TO; }
"." { return Token::DOT; }
"U" { return Token::U; }
"in" { return Token::IN; }
"(" { return Token::LPAR; }
")" { return Token::RPAR; }
--[^"\n"]* ;
[[:alnum:]]+ { copyValue(); return Token::VAR; }
<<EOF>> { return Token::YYEOF; }
