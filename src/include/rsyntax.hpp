#pragma once

// Header files for the raw terms
// Those terms are obtained from the parser
// It is also called pre-syntax

#include "common.hpp"


// With types, we keep track also of the de Bruijn level of the variable,
// and of its source (true: from the source, false: from a named implicit argument)
struct type_t {
    value_ptr typ;
    std::size_t level;

    type_t(value_ptr typ, std::size_t level) : typ{typ}, level{level} {}
};

// The context keep track of the variables seen so far in the scope
// environment: the values of those variables if known
// types: see above
// flags: booleans that tell if the variables comes from a lambda (true) or a let (false)
// level: the current de Bruijn level
struct context_t {

    environment_t environment;
    types_t types;
    std::size_t level; 
    locals_ptr local;
    prunings_t prune;

    context_t() : 
        environment {environment_t()}, 
        types {types_t()}, 
        level {0},
        local {std::make_unique<locals_t>()},
        prune {prunings_t()} {}
    // Add a new rigid variable in the context
    void new_var(name_t, value_ptr);
    // Add a new defined by a let variable with the corresponding type
    void new_val(name_t, term_ptr, value_ptr, term_ptr, value_ptr);
    // Add a new variable from a named implicit argument
    void new_bind(name_t, value_ptr);
    // To properly manage the variables in scope
    void pop(name_t);
    void pop();
};
std::ostream& operator<< (std::ostream& out, context_t& cont);

struct raw_t : std::enable_shared_from_this<raw_t> {

    raw_t() : std::enable_shared_from_this<raw_t>() {}
    virtual ~raw_t() {}

    // for printing, ostreams for overloading << operator
    virtual std::ostream& to_string(std::ostream&);
    // to get a string out of a variable or a hole
    // useful in the parser
    virtual name_t get_name() const;
    // helpers for the parser
    // we need sometimes lists of terms instead of terms
    // but using unions with bison seems not well supported
    // so we encode those lists as terms and we use those 
    // helpers to manage those
    virtual std::vector<name_t>* get_namelist();
    virtual void push_back(raw_ptr);
    // Same hereto build up a term from a list of terms
    virtual raw_ptr auto_build();
    virtual raw_ptr build(raw_ptr);
    // Type check and inferrance
    virtual term_ptr check(context_t&,value_ptr);
    virtual inferrance_t infer(context_t&);
};
std::ostream& operator<< (std::ostream&, raw_t&);

// Variables
struct rvar_t : raw_t {
    name_t name;

    rvar_t(name_t name) : name {name} {}

    std::ostream& to_string(std::ostream&);
    name_t get_name() const;
    inferrance_t infer(context_t&);
};

// lambdas
// implicit ones (riabs) and named implicit one (rnabs)
// inherits from the explicit ones to backward compatibility
// it is also useful since some functions do not care about 
// the type of lambdas
struct rabs_t : raw_t {
    name_t var;
    raw_ptr body;

    rabs_t(name_t var, raw_ptr body) : var {var}, body {body} {}

    std::ostream& to_string(std::ostream&);
    term_ptr check(context_t&,value_ptr);
    inferrance_t infer(context_t&);
};
struct riabs_t : rabs_t {

    riabs_t(name_t var, raw_ptr body) : rabs_t(var,body) {}

    std::ostream& to_string(std::ostream&);
    term_ptr check(context_t&,value_ptr);
};
struct rnabs_t : rabs_t {

    name_t ivar;

    rnabs_t(name_t var, raw_ptr body, name_t ivar) : rabs_t(var,body), ivar {ivar} {}

    std::ostream& to_string(std::ostream&);
    term_ptr check(context_t&,value_ptr);
    inferrance_t infer(context_t&);
};

// Applications
// Same as lambdas
struct rapp_t : raw_t {
    raw_ptr left;
    raw_ptr right;

    rapp_t(raw_ptr left,raw_ptr right) : left {left}, right {right} {}

    std::ostream& to_string(std::ostream&);
    inferrance_t infer(context_t&);
};
struct riapp_t : rapp_t {

    riapp_t(raw_ptr left,raw_ptr right) : rapp_t(left,right) {}

    std::ostream& to_string(std::ostream&);
    inferrance_t infer(context_t&);
};
struct rnapp_t : rapp_t {

    name_t ivar;

    rnapp_t(raw_ptr left,raw_ptr right, name_t ivar) : rapp_t(left,right), ivar {ivar} {}

    std::ostream& to_string(std::ostream&);
    inferrance_t infer(context_t&);
};

// Lets
struct rlet_t : raw_t {
    name_t var;
    raw_ptr typ;
    raw_ptr def;
    raw_ptr body;

    rlet_t(name_t var, 
        raw_ptr typ, 
        raw_ptr def,
        raw_ptr body) : 
        var {var},
        typ {typ},
        def {def}, 
        body {body} 
        {}

    std::ostream& to_string(std::ostream&);
    term_ptr check(context_t&,value_ptr);
    inferrance_t infer(context_t&);
};

// Universes
// Maybe we could have one unique shared pointer to this
struct ru_t : raw_t {

    ru_t() {}
    std::ostream& to_string(std::ostream&);
    inferrance_t infer(context_t&);
};

// Pi types
// Same as lambdas
struct rpi_t : raw_t {
    name_t var;
    raw_ptr typ;
    raw_ptr body;

    rpi_t(name_t var, 
        raw_ptr typ, 
        raw_ptr body) : 
        var {var},
        typ {typ},
        body {body} 
        {}

    std::ostream& to_string(std::ostream&);
    inferrance_t infer(context_t&);
};
struct ripi_t : rpi_t {

    ripi_t(name_t var, raw_ptr typ, raw_ptr body) : rpi_t(var,typ,body) {}

    std::ostream& to_string(std::ostream&);
    inferrance_t infer(context_t&);
};

// Holes
// They basically behaves like variables "_" for the parser
struct rhole_t : raw_t {

    rhole_t() {}
    name_t get_name() const;
    std::ostream& to_string(std::ostream&);
    term_ptr check(context_t&,value_ptr);
    inferrance_t infer(context_t&);
};


// Those are only used in the parser

// Basically, prefixes of pis
struct pibinder_t : raw_t {
    std::vector<name_t> names;
    raw_ptr typ;
    bool icit;

    pibinder_t(std::vector<name_t> names, raw_ptr typ, bool icit): names {names}, typ {typ}, icit {icit} {}
    raw_ptr build(raw_ptr);
};
struct pibinderlist_t : raw_t {
    std::vector<raw_ptr> binders;

    pibinderlist_t() : binders{std::vector<raw_ptr>()} {}
    raw_ptr build(raw_ptr);
    void push_back(raw_ptr);
};

// List of lambda declarations
struct icit : raw_t {
    name_t bind;
    icit(name_t bind) : bind{bind} {}
};
struct iicit : icit {
    iicit(name_t bind) : icit(bind) {}
    raw_ptr build(raw_ptr);
};
struct eicit : icit {
    eicit(name_t bind) : icit(bind) {}
    raw_ptr build(raw_ptr);
};
struct nicit : icit {
    name_t name;
    nicit(name_t bind,name_t name): icit(bind), name{name} {}
    raw_ptr build(raw_ptr );
};
struct icitlist_t : raw_t {
    std::vector<raw_ptr> icits;

    icitlist_t() : icits{std::vector<raw_ptr>()} {}
    raw_ptr build(raw_ptr);
    void push_back(raw_ptr);
};

// To keep track of lists of arguments
struct arg_t : raw_t {
    raw_ptr arg;
    arg_t(raw_ptr arg) : arg{arg} {}
};
struct iarg_t : arg_t {
    iarg_t(raw_ptr arg) : arg_t(arg) {}
    raw_ptr build(raw_ptr);
};
struct earg_t : arg_t {
    earg_t(raw_ptr arg) : arg_t(arg) {}
    raw_ptr build(raw_ptr);
};
struct narg_t : arg_t {
    name_t name;
    narg_t(raw_ptr arg,name_t name): arg_t(arg), name{name} {}
    raw_ptr build(raw_ptr);
};
struct arglist_t : raw_t {
    std::vector<raw_ptr> args;

    arglist_t() : args{std::vector<raw_ptr>()} {}
    raw_ptr build(raw_ptr);
    raw_ptr auto_build();
    void push_back(raw_ptr);
};

// List of variables and holes
// They can be a list of variable declarations in pi or lambda
// or a chain of applications
struct namelist_t : raw_t {
    std::vector<name_t>* names;

    ~namelist_t() {delete names;}

    namelist_t() : names{} {names = new std::vector<name_t>();}
    std::vector<name_t>* get_namelist();
};
