#pragma once
#include "common.hpp"


struct inferrance_t {
    term_ptr term;
    value_ptr typ;

    inferrance_t(term_ptr term, value_ptr typ) : term {term}, typ {typ} {}
};

struct value_t : std::enable_shared_from_this<value_t> {

    virtual ~value_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual value_ptr eval_in_abs(value_ptr);
    virtual term_ptr quote(std::size_t);
    virtual bool conv_VU(std::size_t);
    virtual bool conv_VVAR(std::size_t, std::size_t);
    virtual bool conv_VPI(std::size_t, value_ptr, closure_t&);
    virtual bool conv_VABS(std::size_t, closure_t&);
    virtual bool conv_VAPP(std::size_t, value_ptr, value_ptr);
    virtual bool conv(std::size_t, value_ptr);
    virtual term_ptr check_RABS(context_t&,name_t, raw_ptr);
    virtual inferrance_t infer_RAPP(context_t&,term_ptr,raw_ptr);
    name_t val_in_ctx(context_t&);

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
    std::size_t level;

    vvar_t(std::size_t level) : level {level} {}

    std::ostream& to_string(std::ostream&);
    term_ptr quote(std::size_t);
    bool conv_VVAR(std::size_t, std::size_t);
    bool conv(std::size_t, value_ptr);
};

struct vabs_t : value_t {
    name_t var;
    closure_t body;

    vabs_t(name_t& var, closure_t& closure) : var {var}, body {closure} {}
    vabs_t(name_t& var, environment_t env, term_ptr term) : var {var}, body {closure_t(env,term)} {}

    std::ostream& to_string(std::ostream&);
    value_ptr eval_in_abs(value_ptr);
    term_ptr quote(std::size_t);
    bool conv_VU(std::size_t);
    bool conv_VVAR(std::size_t, std::size_t);
    bool conv_VABS(std::size_t, closure_t&);
    bool conv_VAPP(std::size_t, value_ptr, value_ptr);
    bool conv_VPI(std::size_t, value_ptr, closure_t&);
    bool conv(std::size_t, value_ptr);
};

struct vapp_t : value_t {
    value_ptr left;
    value_ptr right;

    vapp_t(value_ptr left,value_ptr right) : left {left}, right {right} {}
    vapp_t() {}

    std::ostream& to_string(std::ostream&);
    term_ptr quote(std::size_t);
    bool conv_VAPP(std::size_t, value_ptr, value_ptr);
    bool conv(std::size_t, value_ptr);
};

struct vu_t : value_t {

    vu_t() {}
    std::ostream& to_string(std::ostream&);
    term_ptr quote(std::size_t);
    bool conv_VU(std::size_t);
    bool conv(std::size_t, value_ptr);
};

struct vpi_t : value_t {
    name_t var;
    value_ptr typ;
    closure_t body;

    vpi_t(name_t var,
        value_ptr typ,
        closure_t body) : 
        var {var}, typ {typ}, body {body} {}
    vpi_t(name_t var,
        value_ptr typ,
        environment_t env,
        term_ptr term) : 
        var {var}, typ {typ}, body {closure_t(env,term)} {}
    std::ostream& to_string(std::ostream&);
    term_ptr quote(std::size_t);
    bool conv_VPI(std::size_t, value_ptr, closure_t&);
    bool conv(std::size_t, value_ptr);
    term_ptr check_RABS(context_t&,name_t, raw_ptr);
    inferrance_t infer_RAPP(context_t&,term_ptr,raw_ptr);
};

struct vflex_t : value_t {
    std::size_t index;
    spine_t spine;

    vflex_t(std::size_t index) : index {index}, spine {spine_t()} {}
    std::ostream& to_string(std::ostream&);
};

struct vrig_t : value_t {
    std::size_t level;
    spine_t spine;

    vrig_t(std::size_t level) : level {level}, spine {spine_t()} {}
    std::ostream& to_string(std::ostream&);
};