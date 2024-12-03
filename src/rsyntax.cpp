#include <iostream>
#include "rsyntax.hpp"



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

