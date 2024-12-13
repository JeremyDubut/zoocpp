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


%x comment

%%

%{
    using Token = Parser::token;
%}

--[^"\n"]* ;
"{-"            { BEGIN(comment); }
<comment>"-}" { BEGIN(INITIAL); }
<comment>\n   { ++currentLine; }
<comment>.    { }
"\n" { ++currentLine; }
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
";" { return Token::IN; }
"(" { return Token::LPAR; }
")" { return Token::RPAR; }
"{" { return Token::LBRA; }
"}" { return Token::RBRA; }
[[:alnum:]]+ { copyValue(); return Token::VAR; }
<<EOF>> { return Token::YYEOF; }
