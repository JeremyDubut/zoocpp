#include <iostream>
#include "rsyntax.hpp"

#define BIND_CONTEXT(t) \
    environment.push_back(t); \
    types[var] = std::pair<value_ptr,int>(v,level); \
    level++;

std::ostream& operator<< (std::ostream& out, raw_t& term) {return term.to_string(out);}


std::ostream& raw_t::to_string(std::ostream& out) {return out << "Unknown raw term";}
std::ostream& rvar_t::to_string(std::ostream& out) {return out << name;}
std::ostream& rabs_t::to_string(std::ostream& out) {
    return body->to_string(out << "λ" << var << " ");
}
std::ostream& rapp_t::to_string(std::ostream& out) {
    left->to_string(out << "(");
    right->to_string(out << " ");
    return out << ")";
}
std::ostream& rlet_t::to_string(std::ostream& out) {
    typ->to_string(out << "Let " << var << " : ");
    def->to_string(out << " = ");
    return body->to_string(out << " in" << std::endl);
}
std::ostream& ru_t::to_string(std::ostream& out) {
    return out << "𝒰";
}
std::ostream& rpi_t::to_string(std::ostream& out) {
    typ->to_string(out << "(" << var << " : ");
    return body->to_string(out << ") → ");
}

raw_ptr raw_t::update_body(raw_ptr body) {return body;}
raw_ptr rabs_t::update_body(raw_ptr body) {
    this->body = this->body->update_body(body);
    return shared_from_this();
}

std::string raw_t::get_name() {return "Unknown name";}
std::string rvar_t::get_name() {return name;}

void context_t::new_var(std::string var, value_ptr v) {
    BIND_CONTEXT(std::make_shared<vvar_t>(level));
}
void context_t::new_val(std::string var, value_ptr t, value_ptr v) {
    BIND_CONTEXT(t);
}

term_ptr raw_t::check(context_t&,value_ptr) {
    throw "Needs to infer"; // TODO
}
term_ptr rabs_t::check(context_t& cont,value_ptr v) {
    return v->check_RABS(cont,var,body);
}
term_ptr rlet_t::check(context_t& cont,value_ptr v) {
    term_ptr ta = typ->check(cont,std::make_shared<vu_t>());
    value_ptr va = ta->eval(cont.environment);
    term_ptr tt = def->check(cont,va);
    value_ptr vt = tt->eval(cont.environment);
    cont.new_val(var,va,vt);
    term_ptr tu = body->check(cont,v);
    return std::make_shared<let_t>(var,tt,tu);
}

