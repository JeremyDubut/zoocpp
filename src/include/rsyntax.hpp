#pragma once
#include "common.hpp"

struct context_t {

    environment_t environment;
    types_t types;
    flags_t flags;
    std::size_t level; 

    context_t() : environment {environment_t()}, types {types_t()}, flags {flags_t()}, level {0} {}
    void new_var(name_t, value_ptr);
    void new_val(name_t, value_ptr, value_ptr);
    void pop(name_t);
};
std::ostream& operator<< (std::ostream& out, context_t& cont);

struct raw_t : std::enable_shared_from_this<raw_t> {

    virtual ~raw_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual raw_ptr update_body(raw_ptr);
    virtual name_t get_name();
    virtual term_ptr check(context_t&,value_ptr);
    virtual inferrance_t infer(context_t&);

};

std::ostream& operator<< (std::ostream&, raw_t&);

struct rvar_t : raw_t {
    name_t name;

    rvar_t(name_t name) : name {name} {}

    std::ostream& to_string(std::ostream&);
    name_t get_name();
    inferrance_t infer(context_t&);
};

struct rabs_t : raw_t {
    name_t var;
    raw_ptr body;

    rabs_t(name_t var, raw_ptr body) : var {var}, body {body} {}

    std::ostream& to_string(std::ostream&);
    raw_ptr update_body(raw_ptr);
    term_ptr check(context_t&,value_ptr);
    inferrance_t infer(context_t&);
};

struct rapp_t : raw_t {
    raw_ptr left;
    raw_ptr right;

    rapp_t(raw_ptr left,raw_ptr right) : left {left}, right {right} {}

    std::ostream& to_string(std::ostream&);
    inferrance_t infer(context_t&);
};

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

struct ru_t : raw_t {

    ru_t() {}
    std::ostream& to_string(std::ostream&);
    inferrance_t infer(context_t&);
};

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

struct rhole_t : raw_t {

    rhole_t() {}
    std::ostream& to_string(std::ostream&);
    term_ptr check(context_t&,value_ptr);
    inferrance_t infer(context_t&);
};

struct rnl_t : raw_t {

    raw_ptr body;

    rnl_t(raw_ptr body) : body {body} {}
    std::ostream& to_string(std::ostream&);

};
