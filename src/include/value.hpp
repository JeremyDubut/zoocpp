#pragma once

// Header file for values (basically, types)

#include "common.hpp"


// Output type of inferrance
// Basically, a pair of a term and a type
struct inferrance_t {
    term_ptr term;
    value_ptr typ;

    inferrance_t(term_ptr term, value_ptr typ) : term {term}, typ {typ} {}
};


// Values
struct value_t : std::enable_shared_from_this<value_t> {

    virtual ~value_t() {}

    // To print
    virtual std::ostream& to_string(std::ostream&);
    raw_ptr display();
    // Reification
    virtual term_ptr quote(std::size_t);
    // Cases for type checking
    virtual term_ptr check_RABS(context_t&,name_t, raw_ptr);
    virtual term_ptr check_RIABS(context_t&,name_t, raw_ptr);
    virtual term_ptr check_RNABS(context_t&,name_t,name_t, raw_ptr);
    virtual term_ptr check_RAW(context_t&,raw_ptr);
    virtual term_ptr check_LET(context_t&,name_t,raw_ptr,raw_ptr,raw_ptr);
    virtual term_ptr check_HOLE(context_t&);
    // Cases for type inferrance
    virtual std::pair<value_ptr,closure_t> infer_RAPP(context_t&);
    virtual std::pair<value_ptr,closure_t> infer_RINAPP(context_t&);
    // Unification
    virtual void unify(std::size_t,value_ptr);
    virtual void unify_ABS(std::size_t,closure_t&);
    virtual void unify_IABS(std::size_t,closure_t&);
    virtual void unify_U();
    virtual void unify_PI(std::size_t,name_t,value_ptr,closure_t&);
    virtual void unify_IPI(std::size_t,name_t,value_ptr,closure_t&);
    virtual void unify_RIG(std::size_t,std::size_t,spine_t&);
    virtual void unify_FLEX(std::size_t,std::size_t,spine_t&);
    // Basically, beta reduction
    virtual value_ptr vApp(value_ptr,bool);
    value_ptr vAppSp(spine_t&);
    // Helpers for elaboration
    virtual inferrance_t insertUntilName(context_t&,name_t,term_ptr);
    virtual inferrance_t insert(context_t&,term_ptr);
    // Duplicate a value
    // Values are shared in environments and lookpus
    // Most behaviors are consumming
    // Except for the beta reduction in metavariables
    // Which just appends in the spine
    // Duplication is then needed to avoid conflict between spines
    // Another option would be to duplicate the spine everytime something is appended
    virtual value_ptr clone();
    // Forcing the evaluation of a flexible metavariable
    virtual value_ptr force();
    // Helpers for unification
    virtual std::size_t inverse();
    virtual term_ptr rename(std::size_t,renaming_t&);
    void solve(std::size_t,std::size_t,spine_t&);
};
std::ostream& operator<< (std::ostream&, value_t&);
std::ostream& operator<< (std::ostream&, const environment_t&);


// Closures
// Used to avoid evaluating inside lambdas and pis
// This is because we are computing weak head normal form
struct closure_t {
    environment_t environment;
    term_ptr term;

    closure_t(environment_t& environment, term_ptr term) : environment {environment}, term {term} {}
};
std::ostream& operator<< (std::ostream&, const closure_t&);

// Lambdas (explicit, implicit)
struct vabs_t : value_t {
    name_t var;
    closure_t body;

    vabs_t(name_t& var, closure_t& closure) : var {var}, body {closure} {}
    vabs_t(name_t& var, environment_t env, term_ptr term) : var {var}, body {closure_t(env,term)} {}

    std::ostream& to_string(std::ostream&);
    value_ptr vApp(value_ptr,bool);
    term_ptr rename(std::size_t,renaming_t&);
    term_ptr quote(std::size_t);
    void unify(std::size_t,value_ptr);
    void unify_ABS(std::size_t,closure_t&);
    void unify_IABS(std::size_t,closure_t&);
    void unify_RIG(std::size_t,std::size_t,spine_t&);
    void unify_FLEX(std::size_t,std::size_t,spine_t&);
};
struct viabs_t : vabs_t {

    viabs_t(name_t& var, closure_t& closure) : vabs_t(var,closure) {}
    viabs_t(name_t& var, environment_t env, term_ptr term) : vabs_t(var,env,term) {}

    std::ostream& to_string(std::ostream&);
    term_ptr quote(std::size_t);
    term_ptr rename(std::size_t,renaming_t&);
    void unify(std::size_t,value_ptr);
    void unify_RIG(std::size_t,std::size_t,spine_t&);
    void unify_FLEX(std::size_t,std::size_t,spine_t&);
};

// Universes
struct vu_t : value_t {

    vu_t() {}
    std::ostream& to_string(std::ostream&);
    term_ptr quote(std::size_t);
    term_ptr rename(std::size_t,renaming_t&);
    void unify(std::size_t,value_ptr);
    void unify_U();
};

// pi types (explicit, implicit)
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
    term_ptr check_RABS(context_t&,name_t, raw_ptr);
    std::pair<value_ptr,closure_t> infer_RAPP(context_t&);
    std::pair<value_ptr,closure_t> infer_RINAPP(context_t&);
    value_ptr clone();
    term_ptr rename(std::size_t,renaming_t&);
    void unify(std::size_t,value_ptr);
    void unify_PI(std::size_t,name_t,value_ptr,closure_t&);
};
struct vipi_t : vpi_t {

    vipi_t(name_t& var, value_ptr typ, closure_t& closure) : vpi_t(var,typ,closure) {}
    vipi_t(name_t& var, value_ptr typ, environment_t env, term_ptr term) : vpi_t(var,typ,env,term) {}

    std::ostream& to_string(std::ostream&);
    term_ptr quote(std::size_t);
    std::pair<value_ptr,closure_t> infer_RAPP(context_t&);
    std::pair<value_ptr,closure_t> infer_RINAPP(context_t&);
    inferrance_t insertUntilName(context_t&,name_t,term_ptr);
    value_ptr clone();
    term_ptr rename(std::size_t,renaming_t&);
    void unify(std::size_t,value_ptr);
    void unify_PI(std::size_t,name_t,value_ptr,closure_t&);
    void unify_IPI(std::size_t,name_t,value_ptr,closure_t&);
    term_ptr check_RIABS(context_t&,name_t, raw_ptr);
    term_ptr check_RABS(context_t&,name_t, raw_ptr);
    term_ptr check_RNABS(context_t&,name_t,name_t, raw_ptr);
    term_ptr check_LET(context_t&,name_t,raw_ptr,raw_ptr,raw_ptr);
    term_ptr check_HOLE(context_t&);
    term_ptr check_RAW(context_t&,raw_ptr);
    inferrance_t insert(context_t&,term_ptr);
};

// Flexible metavariables
// Basically, holes
struct vflex_t : value_t {
    std::size_t index;
    spine_t spine;

    vflex_t(std::size_t index) : index {index}, spine {spine_t()} {}
    vflex_t(std::size_t index, spine_t& spine) : index {index}, spine {spine} {}
    std::ostream& to_string(std::ostream&);
    term_ptr quote(std::size_t);
    value_ptr vApp(value_ptr,bool);
    value_ptr clone();
    value_ptr force();
    term_ptr rename(std::size_t,renaming_t&);
    void unify(std::size_t,value_ptr);
    void unify_FLEX(std::size_t,std::size_t,spine_t&);
    void unify_U();
    void unify_PI(std::size_t,name_t,value_ptr,closure_t&);
    void unify_IPI(std::size_t,name_t,value_ptr,closure_t&);
    void unify_RIG(std::size_t,std::size_t,spine_t&);
};

// Rigid metavariables
// Basically, variables
struct vrig_t : value_t {
    std::size_t level;
    spine_t spine;

    vrig_t(std::size_t level) : level {level}, spine {spine_t()} {}
    vrig_t(std::size_t level, spine_t& spine) : level {level}, spine {spine} {}
    std::ostream& to_string(std::ostream&);
    term_ptr quote(std::size_t);
    value_ptr vApp(value_ptr,bool);
    value_ptr clone();
    std::size_t inverse();
    term_ptr rename(std::size_t,renaming_t&);
    void unify(std::size_t,value_ptr);
    void unify_RIG(std::size_t,std::size_t,spine_t&);
};