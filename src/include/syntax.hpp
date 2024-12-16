#pragma once
#include "common.hpp"
#include "metavar.hpp"


struct term_t : std::enable_shared_from_this<term_t> {

    virtual ~term_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual value_ptr eval(environment_t&);
    term_ptr nf(environment_t&);
    raw_ptr display();
    virtual raw_ptr display_rec(names_t&);
    virtual inferrance_t insert(context_t&,value_ptr);

};

std::ostream& operator<< (std::ostream&, term_t&);

struct var_t : term_t {
    std::size_t index;

    var_t(std::size_t index) : index {index} {}

    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};

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

struct u_t : term_t {

    u_t() {}

    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);
    raw_ptr display_rec(names_t&);
};

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

struct meta_t : term_t {

    std::size_t index;

    meta_t(): index {metavar_t().id} {}
    meta_t(size_t index) : index {index} {}
    
    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);

};

struct imeta_t : term_t {

    std::size_t index;
    flags_t flags;

    imeta_t(flags_t& flags) : index {metavar_t().id}, flags {flags} {}
    imeta_t() : index {metavar_t().id}, flags {flags_t()} {}
    
    std::ostream& to_string(std::ostream&);
    value_ptr eval(environment_t&);

};