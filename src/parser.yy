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
%token VAR HOLE U
%token TO COLON 
%token LPAR RPAR LBRA RBRA

%nonassoc LAMBDA DOT U NL
%nonassoc VAR HOLE
%left TO 
%nonassoc LPAR LBRA

%expect 1

%%

main: pTm YYEOF { $$ = $1; *term = $$; }

pTm: pLam { std::cout << "pLam" << std::endl; $$ = $1; }
    | pLet { std::cout << "pLet" << std::endl; $$ = $1; }
    | pPi { std::cout << "pPi" << std::endl; $$ = $1; }
    | pFunOrSpine  { std::cout << "pFOS" << std::endl; $$ = $1; } 

pLet: LET VAR EQ pTm IN pTm { 
        std::cout << "pLet impl" << std::endl; 
        $$ = std::make_shared<rlet_t>(($VAR)->get_name(),std::make_shared<rhole_t>(),$4,$6) ; 
    }
    | LET VAR COLON pTm EQ pTm IN pTm { 
        std::cout << "pLet expl" << std::endl; 
        $$ = std::make_shared<rlet_t>($VAR->get_name(),$4,$6,$8) ; 
    }

pFunOrSpine: pSpine TO pTm { 
        std::cout << "pSpine to tm" << std::endl; 
        $$ = std::make_shared<rpi_t>("_",$1,$3); 
    }
    | pSpine { std::cout << "pSpine" << std::endl; $$ = $1; }
pPi: pPiBinderRec TO pTm { 
        std::cout << "pi" << std::endl; 
        $$ = $1->build($3);
        }

pPiBinderRec: pPiBinder { 
        std::cout << "pibinder end" << std::endl; 
        $$ = std::make_shared<pibinderlist_t>(); 
        $$->push_back($1); 
    }
    | pPiBinderRec pPiBinder { 
        std::cout << "pibinder rec" << std::endl; 
        $$ = $1; 
        $$->push_back($2); 
    }
pPiBinder: LBRA pVarList COLON pTm RBRA { 
        std::cout << "pibinder imp typ" << std::endl; 
        $$ = std::make_shared<pibinder_t>(*($2->get_namelist()),$4,true); 
    }
    | LBRA pVarList RBRA { 
        std::cout << "pibinder imp" << std::endl; 
        $$ = std::make_shared<pibinder_t>(*($2->get_namelist()),std::make_shared<rhole_t>(),true); 
    }
    | LPAR pVarList COLON pTm RPAR { 
        std::cout << "pibinder exp" << std::endl; 
        $$ = std::make_shared<pibinder_t>(*($2->get_namelist()),$4,false); 
    }

pSpine: pVarListAtom pArgRec { 
        std::cout << "pSpine" << std::endl; 
        $$ = $1->auto_build();
        $$ = $2->build($$); 
    }
pArgRec: %empty { $$ = std::make_shared<arglist_t>(); }
    | pArgRec pArg pVarListE { 
        std::cout << "pArgRec rec" << std::endl; 
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

pArg: LBRA pTm RBRA { $$ = std::make_shared<iarg_t>($2); }
    | LBRA VAR EQ pTm RBRA { $$ = std::make_shared<narg_t>($4,$VAR->get_name()); }
    | pAtom { $$ = std::make_shared<earg_t>($1); }
pAtom: U { std::cout << "pAtom u" << std::endl; $$ = std::make_shared<ru_t>(); }
    | LPAR pTm RPAR { std::cout << "pArgRec term" << std::endl; $$ = $2; }
pBind: VAR { std::cout << "VAR" << std::endl; $$ = $VAR; }
    | HOLE { std::cout << "HOLE" << std::endl; $$ = std::make_shared<rhole_t>(); }
pVarListAtom: pAtom pVarListE { 
        std::cout << "pVLA atom" << std::endl; 
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
    | pVarList { std::cout << "pVLA VLA" << std::endl; 
        $$ = std::make_shared<arglist_t>(); 
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
        std::cout << "pVL rec" << std::endl; 
        $$ = $1; $$->get_namelist()->push_back($2->get_name()); }
pVarListE: pVarListE pBind { 
        std::cout << "pVLE rec" << std::endl; 
        $$ = $1; $$->get_namelist()->push_back($2->get_name()); }
    | empty { std::cout << "empty" << std::endl; $$ = std::make_shared<namelist_t>();  }
empty: %empty { $$ = std::make_shared<ru_t>();}

pLam: LAMBDA pLamBinderRec DOT pTm { 
        std::cout << "pLam" << std::endl; 
        $$ = $2->build($4);
        }
pLamBinderRec: pLamBinder { 
        std::cout << "pLB rec" << std::endl; 
        $$ = std::make_shared<icitlist_t>(); 
        $$->push_back($1);
    }
    | pLamBinderRec pLamBinder { 
        std::cout << "pLB end" << std::endl; $$ = $1; 
        $$->push_back($2); 
    }
pLamBinder: pBind { $$ = std::make_shared<eicit>($1->get_name()); }
    | LBRA pBind RBRA { $$ = std::make_shared<iicit>($2->get_name()); }
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