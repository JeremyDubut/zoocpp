#pragma once
#include <string>
#include <iostream>
#include <algorithm>

struct value_t;
typedef std::shared_ptr<value_t> value_ptr;
typedef std::vector<value_ptr> environment_t;
struct term_t;
typedef std::shared_ptr<term_t> term_ptr;
struct raw_t;
typedef std::shared_ptr<raw_t> raw_ptr;

struct term_t {

    virtual ~term_t() {}

    virtual std::string to_string();
    virtual value_ptr eval(environment_t&);
    term_ptr nf(environment_t&);
    raw_ptr display();
    virtual raw_ptr display_rec(std::vector<std::string>&);

};

std::ostream& operator<< (std::ostream&, term_t&);

struct var_t : term_t {
    int index;

    var_t(int index) : index {index} {}
    var_t(std::string index) : index {std::stoi(index)} {}

    std::string to_string();
    value_ptr eval(environment_t&);
    raw_ptr display_rec(std::vector<std::string>&);
};

struct abs_t : term_t {
    std::string var;
    term_ptr body;

    abs_t(std::string& var, term_ptr body) : var {var}, body {body} {}
    abs_t() {}

    std::string to_string();
    value_ptr eval(environment_t&);
    raw_ptr display_rec(std::vector<std::string>&);
};

struct app_t : term_t {
    term_ptr left;
    term_ptr right;

    app_t(term_ptr left,term_ptr right) : left {left}, right {right} {}
    app_t() {}

    std::string to_string();
    value_ptr eval(environment_t&);
    raw_ptr display_rec(std::vector<std::string>&);
};

struct let_t : term_t {
    std::string var;
    term_ptr typ;
    term_ptr def;
    term_ptr body;

    let_t(std::string& var, term_ptr typ, term_ptr def, term_ptr body) : var {var}, typ {typ}, def {def}, body {body} {}
    let_t() {}

    std::string to_string();
    value_ptr eval(environment_t&);
    raw_ptr display_rec(std::vector<std::string>&);
};

struct u_t : term_t {

    u_t() {}
    std::string to_string();
    value_ptr eval(environment_t&);
    raw_ptr display_rec(std::vector<std::string>&);
};

struct pi_t : term_t {
    std::string var;
    term_ptr typ;
    term_ptr body;

    pi_t(std::string var,
        term_ptr typ,
        term_ptr body) : 
        var {var}, typ {typ}, body {body} {}
    std::string to_string();
    value_ptr eval(environment_t&);
    raw_ptr display_rec(std::vector<std::string>&);
};