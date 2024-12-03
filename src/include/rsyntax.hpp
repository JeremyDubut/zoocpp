#pragma once
#include <string>
#include <iostream>
#include <algorithm>

struct raw_t;
typedef std::shared_ptr<raw_t> raw_ptr;

struct raw_t : std::enable_shared_from_this<raw_t> {

    virtual ~raw_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual raw_ptr update_body(raw_ptr);
    virtual std::string get_name();

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