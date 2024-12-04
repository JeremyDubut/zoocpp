#pragma once
#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include "value.hpp"

typedef std::map<std::string,std::pair<value_ptr,int>> types_t;
struct context_t {

    environment_t environment;
    types_t types;
    int level; 

    context_t() : environment {environment_t()}, types {types_t()}, level {0} {}
    void new_var(std::string, value_ptr);
    void new_val(std::string, value_ptr, value_ptr);
};

struct raw_t;
typedef std::shared_ptr<raw_t> raw_ptr;

struct raw_t : std::enable_shared_from_this<raw_t> {

    virtual ~raw_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual raw_ptr update_body(raw_ptr);
    virtual std::string get_name();
    virtual term_ptr check(context_t&,value_ptr);

};

std::ostream& operator<< (std::ostream&, raw_t&);

struct rvar_t : raw_t {
    std::string name;

    rvar_t(std::string name) : name {name} {}

    std::ostream& to_string(std::ostream&);
    std::string get_name();
};

struct rabs_t : raw_t {
    std::string var;
    raw_ptr body;

    rabs_t(std::string var, raw_ptr body) : var {var}, body {body} {}

    std::ostream& to_string(std::ostream&);
    raw_ptr update_body(raw_ptr);
    term_ptr check(context_t&,value_ptr);
};

struct rapp_t : raw_t {
    raw_ptr left;
    raw_ptr right;

    rapp_t(raw_ptr left,raw_ptr right) : left {left}, right {right} {}

    std::ostream& to_string(std::ostream&);
};

struct rlet_t : raw_t {
    std::string var;
    raw_ptr typ;
    raw_ptr def;
    raw_ptr body;

    rlet_t(std::string var, 
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
};

struct ru_t : raw_t {

    ru_t() {}
    std::ostream& to_string(std::ostream&);
};

struct rpi_t : raw_t {
    std::string var;
    raw_ptr typ;
    raw_ptr body;

    rpi_t(std::string var, 
        raw_ptr typ, 
        raw_ptr body) : 
        var {var},
        typ {typ},
        body {body} 
        {}

    std::ostream& to_string(std::ostream&);
};