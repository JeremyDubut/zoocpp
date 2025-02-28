%require "3.2"
%language "c++"

%code requires {
    #include "location_t.hpp"
    #include "lexer.hpp"
    #include "common.hpp"
}

%define api.namespace {foo}
%define api.parser.class {Parser}
%define api.value.type { raw_ptr }
%define api.location.type {location_t}

%locations
%define parse.error detailed
%define parse.trace

%header
%verbose

%parse-param {foo::Lexer &lexer}
%parse-param {const bool debug}
%parse-param {raw_ptr* term}

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
    
    #define YYLLOC_DEFAULT(Cur, Rhs, N) foo::calcLocation(Cur, Rhs, N)
    #define yylex lexer.yylex
}


%token LET IN EQ DOT
%token LAMBDA
%token VAR HOLE U BANG
%token TO COLON 
%token LPAR RPAR LBRA RBRA

%nonassoc LAMBDA DOT U NL
%nonassoc VAR HOLE
%left TO 
%right EQ
%nonassoc LPAR LBRA

%expect 0

%%

main: pTm YYEOF { $$ = $1; *term = $$; }

pTm: pLam { $$ = $1; }
    | pLet { $$ = $1; }
    | pPi { $$ = $1; }
    | pFunOrSpine  { $$ = $1; } 

pLet: LET VAR EQ pTm IN pTm { 
        $$ = std::make_shared<rlet_t>(($VAR)->get_name(),std::make_shared<rhole_t>(),$4,$6) ; 
    }
    | LET VAR COLON pTm EQ pTm IN pTm { 
        $$ = std::make_shared<rlet_t>($VAR->get_name(),$4,$6,$8) ; 
    }

pFunOrSpine: pSpine TO pTm { 
        $$ = std::make_shared<rpi_t>("_",$1,$3); 
    }
    | pSpine { $$ = $1; }
pPi: pPiBinderRec TO pTm { 
        $$ = $1->build($3);
        }

pPiBinderRec: pPiBinder { 
        $$ = std::make_shared<pibinderlist_t>(); 
        $$->push_back($1); 
    }
    | pPiBinderRec pPiBinder { 
        $$ = $1; 
        $$->push_back($2); 
    }
pPiBinder: LBRA pVarList COLON pTm RBRA { 
        $$ = std::make_shared<pibinder_t>(*($2->get_namelist()),$4,true); 
    }
    | LBRA pVarList RBRA { 
        $$ = std::make_shared<pibinder_t>(*($2->get_namelist()),std::make_shared<rhole_t>(),true); 
    }
    | LPAR pVarList COLON pTm RPAR { 
        $$ = std::make_shared<pibinder_t>(*($2->get_namelist()),$4,false); 
    }

pSpine: pVarListAtom pArgRec { 
        $$ = $1->auto_build();
        $$ = $2->build($$); 
    }
pArgRec: empty { $$ = std::make_shared<arglist_t>(); }
    | pArgRec pArg pVarListE { 
        $$ = $1; $$->push_back($2); 
        for (name_t it : *($3->get_namelist())) {
            raw_ptr r;
            if (it == "_") {
                r = std::make_shared<rhole_t>();
            }
            else {
                r = std::make_shared<rvar_t>(it);
            }
            $$->push_back(std::make_shared<earg_t>(r));
        }
    }

pArg: LBRA pUnamedArg RBRA { $$ = $2; }
    | LBRA pNamedArg RBRA { $$ = $2; }
    | pAtom { $$ = std::make_shared<earg_t>($1); }
pNamedArg: BANG VAR EQ pTm { $$ = std::make_shared<narg_t>($4,$VAR->get_name()); }
pUnamedArg: pTm { $$ = std::make_shared<iarg_t>($1); }
pAtom: U { $$ = std::make_shared<ru_t>(); }
    | LPAR pTm RPAR { $$ = $2; }
pBind: VAR { $$ = $VAR; }
    | HOLE { $$ = std::make_shared<rhole_t>(); }
pVarListAtom: pAtom pVarListE { 
        $$ = std::make_shared<arglist_t>(); 
        $$->push_back($1); 
        for (name_t it : *($2->get_namelist())) {
            raw_ptr r;
            if (it == "_") {
                r = std::make_shared<rhole_t>();
            }
            else {
                r = std::make_shared<rvar_t>(it);
            }
            $$->push_back(std::make_shared<earg_t>(r));}
        }
    | pVarList { $$ = std::make_shared<arglist_t>(); 
        std::vector<name_t> names = *($1->get_namelist());
        auto it = names.begin();
        if (*it == "_") {
            $$->push_back(std::make_shared<rhole_t>());
        }
        else {
            $$->push_back(std::make_shared<rvar_t>(*it));
        }
        it++;
        while (it!=names.end()) {
            raw_ptr r;
            if (*it == "_") {
                r = std::make_shared<rhole_t>();
            }
            else {
                r = std::make_shared<rvar_t>(*it);
            }
            $$->push_back(std::make_shared<earg_t>(r));
            it++;
        }
    }
pVarList: pVarListE pBind { 
        $$ = $1; $$->get_namelist()->push_back($2->get_name()); }
pVarListE: pVarListE pBind { 
        $$ = $1; $$->get_namelist()->push_back($2->get_name()); }
    | empty { $$ = std::make_shared<namelist_t>();  }
empty: %empty { $$ = std::make_shared<ru_t>();}

pLam: LAMBDA pLamBinderRec DOT pTm { 
        $$ = $2->build($4);
        }
pLamBinderRec: pLamBinder { 
        $$ = std::make_shared<icitlist_t>(); 
        $$->push_back($1);
    }
    | pLamBinderRec pLamBinder { 
        $$->push_back($2); 
    }
pLamBinder: pBind { $$ = std::make_shared<eicit>($1->get_name()); }
    | LPAR pBind COLON pTm RPAR { $$ = std::make_shared<teicit>($2->get_name(),$4); }
    | LBRA pBind RBRA { $$ = std::make_shared<iicit>($2->get_name()); }
    | LBRA pBind COLON pTm RBRA { $$ = std::make_shared<tiicit>($2->get_name(),$4); }
    | LBRA VAR EQ pBind RBRA { $$ = std::make_shared<nicit>($4->get_name(),$VAR->get_name()); }

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