#pragma once
#include <string>
#include <vector>
#include <memory>
#include "syntax.hpp"

struct value_t : std::enable_shared_from_this<value_t> {

    virtual ~value_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual std::shared_ptr<value_t> eval_in_abs(std::shared_ptr<value_t>);
    virtual std::shared_ptr<term_t> quote(int);

};
std::ostream& operator<< (std::ostream&, value_t&);
std::ostream& operator<< (std::ostream&, const environment_t&);

struct closure_t {
    environment_t environment;
    std::shared_ptr<term_t> term;

    closure_t(environment_t& environment, std::shared_ptr<term_t> term) : environment {environment}, term {term} {}
};
std::ostream& operator<< (std::ostream&, const closure_t&);

struct vvar_t : value_t {
    int level;

    vvar_t(int level) : level {level} {}

    std::ostream& to_string(std::ostream&);
    std::shared_ptr<term_t> quote(int);
};

struct vabs_t : value_t {
    std::string var;
    closure_t body;

    vabs_t(std::string& var, closure_t& closure) : var {var}, body {closure} {}
    vabs_t(std::string& var, environment_t env, std::shared_ptr<term_t> term) : var {var}, body {closure_t(env,term)} {}

    std::ostream& to_string(std::ostream&);
    std::shared_ptr<value_t> eval_in_abs(std::shared_ptr<value_t>);
    std::shared_ptr<term_t> quote(int);
};

struct vapp_t : value_t {
    std::shared_ptr<value_t> left;
    std::shared_ptr<value_t> right;

    vapp_t(std::shared_ptr<value_t> left,std::shared_ptr<value_t> right) : left {left}, right {right} {}
    vapp_t() {}

    std::ostream& to_string(std::ostream&);
    std::shared_ptr<term_t> quote(int);
};

struct vu_t : value_t {

    vu_t() {}
    std::ostream& to_string(std::ostream&);
    std::shared_ptr<term_t> quote(int);
};

struct vpi_t : value_t {
    std::string var;
    std::shared_ptr<value_t> typ;
    closure_t body;

    vpi_t(std::string var,
        std::shared_ptr<value_t> typ,
        closure_t body) : 
        var {var}, typ {typ}, body {body} {}
    vpi_t(std::string var,
        std::shared_ptr<value_t> typ,
        environment_t env,
        std::shared_ptr<term_t> term) : 
        var {var}, typ {typ}, body {closure_t(env,term)} {}
    std::ostream& to_string(std::ostream&);
    std::shared_ptr<term_t> quote(int);
};