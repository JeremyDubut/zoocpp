#pragma once
#include <string>
#include <vector>
#include <memory>
#include "syntax.hpp"

struct closure_t;
struct raw_t;
typedef std::shared_ptr<raw_t> raw_ptr;
struct context_t;

struct inferrance_t {
    term_ptr term;
    value_ptr typ;

    inferrance_t(term_ptr term, value_ptr typ) : term {term}, typ {typ} {}
};

struct value_t : std::enable_shared_from_this<value_t> {

    virtual ~value_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual value_ptr eval_in_abs(value_ptr);
    virtual term_ptr quote(int);
    virtual bool conv_VU(int);
    virtual bool conv_VVAR(int, int);
    virtual bool conv_VPI(int, value_ptr, closure_t&);
    virtual bool conv_VABS(int, closure_t&);
    virtual bool conv_VAPP(int, value_ptr, value_ptr);
    virtual bool conv(int, value_ptr);
    virtual term_ptr check_RABS(context_t&,std::string, raw_ptr);
    virtual inferrance_t infer_RAPP(context_t&,term_ptr,raw_ptr);
    std::string val_in_ctx(context_t&);

};
std::ostream& operator<< (std::ostream&, value_t&);
std::ostream& operator<< (std::ostream&, const environment_t&);

struct closure_t {
    environment_t environment;
    term_ptr term;

    closure_t(environment_t& environment, term_ptr term) : environment {environment}, term {term} {}
};
std::ostream& operator<< (std::ostream&, const closure_t&);

struct vvar_t : value_t {
    int level;

    vvar_t(int level) : level {level} {}

    std::ostream& to_string(std::ostream&);
    term_ptr quote(int);
    bool conv_VVAR(int, int);
    bool conv(int, value_ptr);
};

struct vabs_t : value_t {
    std::string var;
    closure_t body;

    vabs_t(std::string& var, closure_t& closure) : var {var}, body {closure} {}
    vabs_t(std::string& var, environment_t env, term_ptr term) : var {var}, body {closure_t(env,term)} {}

    std::ostream& to_string(std::ostream&);
    value_ptr eval_in_abs(value_ptr);
    term_ptr quote(int);
    bool conv_VU(int);
    bool conv_VVAR(int, int);
    bool conv_VABS(int, closure_t&);
    bool conv_VAPP(int, value_ptr, value_ptr);
    bool conv_VPI(int, value_ptr, closure_t&);
    bool conv(int, value_ptr);
};

struct vapp_t : value_t {
    value_ptr left;
    value_ptr right;

    vapp_t(value_ptr left,value_ptr right) : left {left}, right {right} {}
    vapp_t() {}

    std::ostream& to_string(std::ostream&);
    term_ptr quote(int);
    bool conv_VAPP(int, value_ptr, value_ptr);
    bool conv(int, value_ptr);
};

struct vu_t : value_t {

    vu_t() {}
    std::ostream& to_string(std::ostream&);
    term_ptr quote(int);
    bool conv_VU(int);
    bool conv(int, value_ptr);
};

struct vpi_t : value_t {
    std::string var;
    value_ptr typ;
    closure_t body;

    vpi_t(std::string var,
        value_ptr typ,
        closure_t body) : 
        var {var}, typ {typ}, body {body} {}
    vpi_t(std::string var,
        value_ptr typ,
        environment_t env,
        term_ptr term) : 
        var {var}, typ {typ}, body {closure_t(env,term)} {}
    std::ostream& to_string(std::ostream&);
    term_ptr quote(int);
    bool conv_VPI(int, value_ptr, closure_t&);
    bool conv(int, value_ptr);
    term_ptr check_RABS(context_t&,std::string, raw_ptr);
    inferrance_t infer_RAPP(context_t&,term_ptr,raw_ptr);
};