#include <iostream>
#include "syntax.hpp"
#include "value.hpp"


std::string term_t::to_string() {return "Unknown term";}
std::string var_t::to_string() {return "Var"+std::to_string(index);}
std::string abs_t::to_string() {
    return "λ "+body->to_string();}
std::string app_t::to_string() {
    return "("+left->to_string()+" "+right->to_string()+")";}
std::string let_t::to_string() {
    return "Let "+def->to_string()+" in\n"+body->to_string();}
std::string u_t::to_string() {return "𝒰";}
std::string pi_t::to_string() {
    return "("+var+" : "+typ->to_string()+") → "+body->to_string();}
std::ostream& operator<< (std::ostream& out, term_t& term) {
    out << term.to_string();
    return out;
}

value_ptr term_t::eval(environment_t&) {
    throw "Evaluation of an unknown term.";
}
value_ptr var_t::eval(environment_t& env) {
    return env.at(env.size()-1-index);
}
value_ptr abs_t::eval(environment_t& env) {
    return std::make_shared<vabs_t>(var,env,body);
}
value_ptr let_t::eval(environment_t& env) {
    env.push_back(def->eval(env));
    return body->eval(env);
}
value_ptr app_t::eval(environment_t& env) {
    return left->eval(env)->eval_in_abs(right->eval(env));
    // return std::make_shared<vapp_t>(*left.eval(env),*right.eval(env));
}
value_ptr u_t::eval(environment_t&) {
    return std::make_shared<vu_t>();
}
value_ptr pi_t::eval(environment_t& env) {
    return std::make_shared<vpi_t>(var,typ->eval(env),env,body);
}

term_ptr term_t::nf(environment_t& env) {
    int length = env.size();
    return eval(env)->quote(length);
}