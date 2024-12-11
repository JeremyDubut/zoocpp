%require "3.2"
%language "c++"

%code requires {
    #include "location_t.hpp"
    #include "lexer.hpp"
    #include "common.hpp"
}
%code {
    struct pibinder_t {
        std::vector<name_t> names;
        raw_ptr typ;
        bool icit;

        pibinder_t(std::vector<name_t> names, raw_ptr typ, bool icit): names {names}, typ {typ}, icit {icit} {}
    };

    raw_ptr pifoldr(std::vector<pibinder_t>& v, raw_ptr r) {
        if (v.empty()) {
            return r;
        }
        pibinder_t pb = *(v.end()-1);
        v.pop_back();
        raw_ptr res = pifoldr(v,r);
        while (!pb.names.empty()) {
            name_t var = *(pb.names.end()-1);
            pb.names.pop_back();
            if (pb.icit) {
                res = std::make_shared<ripi_t>(var,pb.typ,res);
            }
            else {
                res = std::make_shared<rpi_t>(var,pb.typ,res);
            }
        }
        return res;
    }

    struct icit {
        name_t bind;
        icit(name_t bind) : bind{bind} {}
        virtual raw_ptr rptr(raw_ptr) {
            throw "Wrong type of icit";
        }
    };
    struct iicit : icit {
        iicit(name_t bind) : icit(bind) {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<riabs_t>(bind,r);
        }
    };
    struct eicit : icit {
        eicit(name_t bind) : icit(bind) {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<rabs_t>(bind,r);
        }
    };
    struct nicit : icit {
        name_t name;
        nicit(name_t bind,name_t name): icit(bind), name{name} {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<rnabs_t>(bind,r,name);
        }
    };

    raw_ptr lamfoldr(std::vector<icit>& v, raw_ptr r) {
        raw_ptr res = r;
        while (!v.empty()) {
            icit d = *(v.end()-1);
            res = d.rptr(res);
        }
        return res;
    }


    struct arg_t {
        raw_ptr arg;
        arg_t(raw_ptr arg) : arg{arg} {}
        virtual raw_ptr rptr(raw_ptr) {
            throw "Wrong type of icit";
        }
    };
    struct iarg_t : arg_t {
        iarg_t(raw_ptr arg) : arg_t(arg) {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<riapp_t>(r,arg);
        }
    };
    struct earg_t : arg_t {
        earg_t(raw_ptr arg) : arg_t(arg) {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<rapp_t>(r,arg);
        }
    };
    struct narg_t : arg_t {
        name_t name;
        narg_t(raw_ptr arg,name_t name): arg_t(arg), name{name} {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<rnapp_t>(r,arg,name);
        }
    };

    raw_ptr appfoldl(raw_ptr r, std::vector<arg_t>& v) {
        raw_ptr res = r;
        for (arg_t arg : v) {
            res = arg.rptr(r);
        }
        return res;
    }
}

%define api.namespace {foo}
%define api.parser.class {Parser}
%define api.location.type {location_t}

%locations
%define parse.error detailed
%define parse.trace

%header
%verbose

%parse-param {Lexer &lexer}
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
    
    #define YYLLOC_DEFAULT(Cur, Rhs, N) calcLocation(Cur, Rhs, N)
    #define yylex lexer.yylex
}

%union {
    raw_ptr term;
    foo::pibinder_t pb;
    std::vector<foo::pibinder_t> pbs;
    std::vector<name_t> names;
    foo::icit ic;
    std::vector<foo::icit> ics;
    name_t name;
    foo::arg_t arg;
    std::vector<foo::arg_t> args;
}

%token LET IN EQ DOT
%token LAMBDA
%token <raw_ptr> VAR 
%token HOLE U
%token TO COLON 
%token LPAR RPAR RBRA LBRA

%type <raw_ptr> pTm pLam pLet pPi pFunOrSpine pSpine pAtom main
%type <foo::pibinder_t> pPiBinder
%type <std::vector<foo::pibinder_t>> pPiBinderRec
%type <std::vector<name_t>> pVarList
%type <foo::icit> pLamBinder
%type <std::vector<foo::icit>> pLamBinderRec
%type <name_t> pBind
%type <foo::arg_t> pArg
%type <std::vector<foo::arg_t>> pArgRec

%expect 0

%%

main: pTm YYEOF { $$ = $1; *term = $$; }
pTm: pLam { $$ = $1; }
    | pLet { $$ = $1; }
    | pFunOrSpine  { $$ = $1; } 
pLet: LET VAR EQ pTm IN pTm { $$ = std::make_shared<rlet_t>(($VAR)->get_name(),std::make_shared<rhole_t>(),$4,$6) ; }
    | LET VAR COLON pTm EQ pTm IN pTm { $$ = std::make_shared<rlet_t>($VAR->get_name(),$4,$6,$8) ; }
pFunOrSpine: pSpine { $$ = $1; }
    | pSpine TO pTm { $$ = std::make_shared<rpi_t>("_",$1,$3); }
pPi: pPiBinderRec TO pTm { $$ = pifoldr($1,$3); }
pPiBinderRec: pPiBinder { $$ = std::vector<pibinder_t>(); $$.push_back($1); }
    | pPiBinderRec pPiBinder { $$ = $1; $$.push_back($2); }
pPiBinder: LBRA pVarList COLON pTm RBRA { $$ = pibinder_t($2,$4,true); }
    | LBRA pVarList RBRA { $$ = pibinder_t($2,std::make_shared<rhole_t>(),true); }
    | LPAR pVarList COLON pTm RPAR { $$ = pibinder_t($2,$4,false); }
    | LPAR pVarList RPAR { $$ = pibinder_t($2,std::make_shared<rhole_t>(),false); }
pLam: LAMBDA pLamBinderRec DOT pTm { $$ = lamfoldr($2,$4); }
pLamBinderRec: pLamBinder { $$ = std::vector<icit>(); $$.push_back($1); }
    | pLamBinderRec pLamBinder { $$ = $1; $$.push_back($2); }
pLamBinder: pBind { $$ = eicit($1); }
    | LBRA pBind RBRA { $$ = iicit($2); }
    | LBRA VAR EQ pBind RBRA { $$ = nicit($4,$VAR->get_name()); }
pSpine: pAtom pArgRec { $$ = appfoldl($1,$2); }
pArgRec: %empty { $$ = std::vector<arg_t>(); }
    | pArgRec pArg { $$ = $1; $$.push_back($2); }
pArg: LBRA VAR EQ pTm RBRA { $$ = narg_t($4,$VAR->get_name()); }
    | LBRA pTm RBRA { $$ = iarg_t($2); }
    | pAtom { $$ = earg_t($1); }
pAtom: VAR { $$ = $VAR; }
    | U { $$ = std::make_shared<ru_t>(); }
    | HOLE { $$ = std::make_shared<rhole_t>(); }
    | LPAR pTm RPAR { $$ = $2; }
pBind: VAR { $$ = $VAR->get_name(); }
    | HOLE { $$ = "_"; }
pVarList: pBind { $$ = std::vector<name_t>(); $$.pushback($1); }
    | pVarList pBind { $$ = $1; $$.push_back($2); }

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