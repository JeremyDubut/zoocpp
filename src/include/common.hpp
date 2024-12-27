#pragma once

#ifdef DEBUG
    #define LOG(x) std::cout << x << std::endl
#else
    #define LOG(x)
#endif


// Some common types and macros

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <optional>
#include <iostream>
#include <sstream>
#include <unordered_set>




struct value_t;
struct term_t;
struct raw_t;
struct context_t;
struct inferrance_t;
struct closure_t;
struct renaming_t;
struct type_t;
struct pibinder_t;
struct icit;
struct arg_t;
struct metaentry_t;
struct locals_t;
struct checkentry_t;


// We decided to use share pointers because terms are shared during 
// evaluation and inferrance through spines and environments
typedef std::shared_ptr<value_t> value_ptr;
typedef std::shared_ptr<term_t> term_ptr;
typedef std::shared_ptr<raw_t> raw_ptr;
typedef std::shared_ptr<locals_t> locals_ptr;
typedef std::shared_ptr<metaentry_t> meta_ptr;
typedef std::shared_ptr<checkentry_t> check_ptr;

// We keep track of types as a map from variable names to a vector
// keeping the type of the occurences of this variable
// we need a vector because a variable could be binded several times
// except for named arguments, we consider that the last bind is the 
// current occurence in the scope
typedef std::unordered_map<std::string,std::vector<type_t>> types_t;

typedef std::vector<value_ptr> environment_t;
typedef std::string name_t;
typedef std::vector<name_t> names_t;
typedef std::vector<meta_ptr> metadata_t;
typedef std::vector<check_ptr> checkdata_t;
typedef std::vector<bool> flags_t;
typedef std::vector<std::pair<value_ptr,bool>> spine_t;
typedef std::unordered_map<std::size_t,std::size_t> renamingFun_t;
enum pruning_t { Implicit, Explicit, None };
typedef std::vector<pruning_t> prunings_t;
enum status_t { OK, OK_NonRenaming, NeedsPruning};
typedef std::vector<std::pair<std::optional<term_ptr>,bool>> tspine_t;
typedef std::unordered_set<std::size_t> block_t;


// Locals
struct locals_t {

    virtual ~locals_t() {}
    virtual term_ptr closety(term_ptr);
    virtual term_ptr closetm(term_ptr);
    virtual locals_ptr pop();

};
struct lbind_t : locals_t {

    locals_ptr mcl;
    name_t var;
    term_ptr typ;

    lbind_t(locals_ptr mcl, name_t& var, term_ptr typ) : mcl{mcl}, var{var}, typ{typ} {}
    term_ptr closety(term_ptr);
    term_ptr closetm(term_ptr);
    locals_ptr pop();

};
struct ldefine_t : locals_t {

    locals_ptr mcl;
    name_t var;
    term_ptr typ;
    term_ptr def;

    ldefine_t(locals_ptr mcl, name_t& var, term_ptr typ, term_ptr def) : mcl{mcl}, var{var}, typ{typ}, def{def} {}
    term_ptr closety(term_ptr);
    term_ptr closetm(term_ptr);
    locals_ptr pop();

};

// some common macros
#define NEWMETA(typ) std::make_shared<meta_t>(metavar_t(typ).id)
#define CLOSED(typ) cont.local->closety(typ->quote(cont.level))->eval()
#define FRESHMETA(typ) std::make_shared<appp_t>(NEWMETA(CLOSED(typ)),cont.prune)
#define VU std::make_shared<vu_t>()
#define CAPP(v,body,val) \
    body.environment.push_back(v); \
    value_ptr val = body.term->eval(body.environment); \
    body.environment.pop_back(); 

