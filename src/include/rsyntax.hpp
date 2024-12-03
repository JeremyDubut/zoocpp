#pragma once
#include <string>
#include <iostream>
#include <algorithm>


struct raw_t : std::enable_shared_from_this<raw_t> {

    virtual ~raw_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual std::shared_ptr<raw_t> update_body(std::shared_ptr<raw_t>);
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
    std::shared_ptr<raw_t> body;

    rabs_t(std::string var, std::shared_ptr<raw_t> body) : var {var}, body {body} {}

    std::ostream& to_string(std::ostream&);
    std::shared_ptr<raw_t> update_body(std::shared_ptr<raw_t>);
};

struct rapp_t : raw_t {
    std::shared_ptr<raw_t> left;
    std::shared_ptr<raw_t> right;

    rapp_t(std::shared_ptr<raw_t> left,std::shared_ptr<raw_t> right) : left {left}, right {right} {}

    std::ostream& to_string(std::ostream&);
};

struct rlet_t : raw_t {
    std::string var;
    std::shared_ptr<raw_t> typ;
    std::shared_ptr<raw_t> def;
    std::shared_ptr<raw_t> body;

    rlet_t(std::string var, 
        std::shared_ptr<raw_t> typ, 
        std::shared_ptr<raw_t> def,
        std::shared_ptr<raw_t> body) : 
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
    std::shared_ptr<raw_t> typ;
    std::shared_ptr<raw_t> body;

    rpi_t(std::string var, 
        std::shared_ptr<raw_t> typ, 
        std::shared_ptr<raw_t> body) : 
        var {var},
        typ {typ},
        body {body} 
        {}

    std::ostream& to_string(std::ostream&);
};