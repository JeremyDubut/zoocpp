#pragma once
#include "common.hpp"
#include "metavar.hpp"



// Terms used during the inferrance/check
// They are the syntax
struct term_t : std::enable_shared_from_this<term_t> {

    virtual ~term_t() {}

    // To print
    virtual std::ostream& to_string(std::ostream&);
    // Evaluate the variables, the applications, ...
    value_ptr eval();
    virtual value_ptr eval(environment_t&);
    // Weak head normal form
    term_ptr nf(environment_t&);
    // A better display to remove the debruijn indices
    raw_ptr display();
    virtual raw_ptr display_rec(names_t&);
    // helper for the elaboration
    virtual inferrance_t insert(context_t&,value_ptr);

};

std::ostream& operator<< (std::ostream&, term_t&);

// Variables with debruijn index
struct var_t : term_t {
    std::size_t index;

    var_t(std::size_t index) : index {index} {}

    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};

// Lambdas (explicit, implict)
struct abs_t : term_t {
    name_t var;
    term_ptr body;

    abs_t(name_t& var, term_ptr body) : var {var}, body {body} {}
    abs_t() {}

    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};
struct iabs_t : abs_t {

    iabs_t(name_t var, term_ptr body) : abs_t(var,body) {}
    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
    inferrance_t insert(context_t&,value_ptr);
};

// Applications (explicit, implicit arguments)
struct app_t : term_t {
    term_ptr left;
    term_ptr right;

    app_t(term_ptr left,term_ptr right) : left {left}, right {right} {}
    app_t() {}

    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};
struct iapp_t : app_t {

    iapp_t(term_ptr left, term_ptr right) : app_t(left,right) {}
    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};

// Lets
struct let_t : term_t {
    name_t var;
    term_ptr typ;
    term_ptr def;
    term_ptr body;

    let_t(name_t& var, term_ptr typ, term_ptr def, term_ptr body) : var {var}, typ {typ}, def {def}, body {body} {}
    let_t() {}

    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};

// Universes
struct u_t : term_t {

    u_t() {}

    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};

// Pi types (explicit, implicit)
struct pi_t : term_t {
    name_t var;
    term_ptr typ;
    term_ptr body;

    pi_t(name_t var,
        term_ptr typ,
        term_ptr body) : 
        var {var}, typ {typ}, body {body} {}
        
    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};
struct ipi_t : pi_t {

    ipi_t(name_t var, term_ptr typ, term_ptr body) : pi_t(var,typ,body) {}
    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};

// Metavariables
// Constructors gloablly create them
// The term itself only contains the index
struct meta_t : term_t {

    std::size_t index;

    meta_t(value_ptr typ): index {metavar_t(typ).id} {}
    meta_t(size_t index) : index {index} {}
    
    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);

};

// Metavariables, inserted during elaboration
// Constructors gloablly create them
// The term itself only contains the index and flags
// Flags are just to know if variables are bound (true) or defined by let (false)
struct imeta_t : term_t {

    std::size_t index;
    flags_t flags;

    imeta_t(value_ptr typ, flags_t& flags) : index {metavar_t(typ).id}, flags {flags} {}
    imeta_t(value_ptr typ) : index {metavar_t(typ).id}, flags {flags_t()} {}
    
    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);

};

// Application-prunings
struct appp_t : term_t {

    term_ptr left;
    prunings_t prune;

    appp_t(term_ptr left) : left{left}, prune{prunings_t()} {}
    appp_t(term_ptr left, prunings_t& prune) : left{left}, prune{prune} {}
    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);

};