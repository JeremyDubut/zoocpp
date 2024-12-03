%require "3.2"
%language "c++"

%code requires {
    #include <string>
    #include "location_t.hpp"
    #include "lexer.hpp"
    #include "rsyntax.hpp"
}

%define api.namespace {foo}
%define api.parser.class {Parser}
%define api.value.type {std::shared_ptr<raw_t>}
%define api.location.type {location_t}

%locations
%define parse.error detailed
%define parse.trace

%header
%verbose

%parse-param {Lexer &lexer}
%parse-param {const bool debug}
%parse-param {std::shared_ptr<raw_t>* term}

%initial-action
{
    #if YYDEBUG != 0
        set_debug_level(debug);
    #endif
};
%start main

%code {
    namespace foo
    {
        template<typename RHS>
        void calcLocation(location_t &current, const RHS &rhs, const std::size_t n);
    }
    
    #define YYLLOC_DEFAULT(Cur, Rhs, N) calcLocation(Cur, Rhs, N)
    #define yylex lexer.yylex
}

%token LET IN COLON EQ TO DOT U
%token LAMBDA
%token VAR
%token EF
%token APP
%token LPAR RPAR

%nonassoc LAMBDA DOT U
%nonassoc VAR LPAR
%right TO
%nonassoc APP

%expect 0

%%

main: def YYEOF { $$ = $1; *term = $$; }
def: LET VAR COLON body EQ body IN def { $$ = std::make_shared<rlet_t>($2->get_name(),$4,$6,$8) ; }
    | body { $$ = $1; }
body: LAMBDA varlist DOT body { $$ = $2; $$->update_body($4) ; }
    | LPAR body RPAR { $$ = $2; }
    | U { $$ = std::make_shared<ru_t>(); }
    | VAR { $$ = $VAR; };
    | LPAR VAR COLON body RPAR TO body { $$ = std::make_shared<rpi_t>($2->get_name(),$4,$7); }
    | body TO body { $$ = std::make_shared<rpi_t>("_",$1,$3); }
    | body body %prec APP { $$ = std::make_shared<rapp_t>($1,$2); }
varlist: VAR { $$ = std::make_shared<rabs_t>($1->get_name(),std::make_shared<rvar_t>("_")); }
    | VAR varlist { $$ = std::make_shared<rabs_t>($1->get_name(),$2); }

%%

namespace foo
{
    template<typename RHS>
    inline void calcLocation(location_t &current, const RHS &rhs, const std::size_t n)
    {
        current = location_t(YYRHSLOC(rhs, 1).first, YYRHSLOC(rhs, n).second);
    }
    
    void Parser::error(const location_t &location, const std::string &message)
    {
        std::cerr << "Error at lines " << location << ": " << message << std::endl;
    }
}