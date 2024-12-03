#pragma once
#include <string>
#include <iostream>
#include <algorithm>

struct value_t;
typedef std::vector<std::shared_ptr<value_t>> environment_t;

struct term_t {

    virtual ~term_t() {}

    virtual std::string to_string();
    virtual std::shared_ptr<value_t> eval(environment_t&);
    std::shared_ptr<term_t> nf(environment_t&);

};

std::ostream& operator<< (std::ostream&, term_t&);

struct var_t : term_t {
    int index;

    var_t(int index) : index {index} {}
    var_t(std::string index) : index {std::stoi(index)} {}

    std::string to_string();
    std::shared_ptr<value_t> eval(environment_t&);
};

struct abs_t : term_t {
    std::string var;
    std::shared_ptr<term_t> body;

    abs_t(std::string& var, std::shared_ptr<term_t> body) : var {var}, body {body} {}
    abs_t() {}

    std::string to_string();
    std::shared_ptr<value_t> eval(environment_t&);
};

struct app_t : term_t {
    std::shared_ptr<term_t> left;
    std::shared_ptr<term_t> right;

    app_t(std::shared_ptr<term_t> left,std::shared_ptr<term_t> right) : left {left}, right {right} {}
    app_t() {}

    std::string to_string();
    std::shared_ptr<value_t> eval(environment_t&);
};

struct let_t : term_t {
    std::string var;
    std::shared_ptr<term_t> def;
    std::shared_ptr<term_t> body;

    let_t(std::string& var, std::shared_ptr<term_t> def,std::shared_ptr<term_t> body) : var {var}, def {def}, body {body} {}
    let_t() {}

    std::string to_string();
    std::shared_ptr<value_t> eval(environment_t&);
};

struct u_t : term_t {

    u_t() {}
    std::string to_string();
    std::shared_ptr<value_t> eval(environment_t&);
};

struct pi_t : term_t {
    std::string var;
    std::shared_ptr<term_t> typ;
    std::shared_ptr<term_t> body;

    pi_t(std::string var,
        std::shared_ptr<term_t> typ,
        std::shared_ptr<term_t> body) : 
        var {var}, typ {typ}, body {body} {}
    std::string to_string();
    std::shared_ptr<value_t> eval(environment_t&);
};