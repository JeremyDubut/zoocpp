#pragma once
#include "common.hpp"

struct type_t {
    value_ptr typ;
    std::size_t level;
    bool source;

    type_t(value_ptr typ, std::size_t level, bool source) : typ{typ}, level{level}, source{source} {}
};
struct context_t {

    environment_t environment;
    types_t types;
    flags_t flags;
    std::size_t level; 

    context_t() : environment {environment_t()}, types {types_t()}, flags {flags_t()}, level {0} {}
    void new_var(name_t, value_ptr);
    void new_val(name_t, value_ptr, value_ptr);
    void new_bind(name_t, value_ptr);
    void pop(name_t);
};
std::ostream& operator<< (std::ostream& out, context_t& cont);

struct pibinder_t;
struct icit;
struct arg_t;

struct raw_t : std::enable_shared_from_this<raw_t> {

    raw_t() : std::enable_shared_from_this<raw_t>() {}
    virtual ~raw_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual raw_ptr update_body(raw_ptr);
    virtual name_t get_name() const;
    virtual raw_ptr build(raw_ptr);
    virtual void push_back(raw_ptr);
    virtual std::vector<name_t>* get_namelist();
    virtual raw_ptr auto_build();
    // virtual std::vector<name_t> get_names() const;
    // virtual std::vector<raw_ptr>* get_binderlist() const;
    // virtual raw_ptr build_icit(raw_ptr) const;
    // virtual raw_ptr build_pi(raw_ptr);
    // virtual std::vector<raw_ptr>* get_icitlist() const;
    // virtual arg_t build_arg(raw_ptr) const;
    // virtual std::vector<raw_ptr>* get_arglist() const;
    virtual term_ptr check(context_t&,value_ptr);
    virtual inferrance_t infer(context_t&);

};

std::ostream& operator<< (std::ostream&, raw_t&);

struct rvar_t : raw_t {
    name_t name;

    rvar_t(name_t name) : name {name} {}

    std::ostream& to_string(std::ostream&);
    name_t get_name() const;
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
struct ripi_t : rpi_t {

    ripi_t(name_t var, raw_ptr typ, raw_ptr body) : rpi_t(var,typ,body) {}

    std::ostream& to_string(std::ostream&);
    inferrance_t infer(context_t&);
};

struct rhole_t : raw_t {

    rhole_t() {}
    name_t get_name() const;
    std::ostream& to_string(std::ostream&);
    term_ptr check(context_t&,value_ptr);
    inferrance_t infer(context_t&);
};

struct rnl_t : raw_t {

    raw_ptr body;

    rnl_t(raw_ptr body) : body {body} {}
    std::ostream& to_string(std::ostream&);

};

// used only for parsing


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

struct namelist_t : raw_t {
    std::vector<name_t>* names;

    ~namelist_t() {delete names;}

    namelist_t() : names{} {names = new std::vector<name_t>();}
    std::vector<name_t>* get_namelist();
};
