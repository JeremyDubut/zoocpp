#include <iostream>
#include <sstream>
#include "rsyntax.hpp"

#define BIND_CONTEXT(t) \
    environment.push_back(t); \
    types[var] = std::pair<value_ptr,int>(v,level); \
    level++;
#define VU std::make_shared<vu_t>()

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
void context_t::pop(std::string var) {
    environment.pop_back();
    level--;
    types.erase(var);
}

std::ostream& operator<< (std::ostream& out, context_t& cont) {
    out << std::endl << "Context" << std::endl;
    out << "- Environment" << std::endl;
    out << cont.environment << std::endl;
    out << "- Level: " << cont.level << std::endl;
    out << "- Types" << std::endl;
    for (auto it = cont.types.begin(); it != cont.types.end(); it++) {
        out << it->first << " " << *((it->second).first) << " " << (it->second).second << std::endl;
    }
    return out;
}

term_ptr raw_t::check(context_t& cont,value_ptr typ) {
    std::cout << "Inferring type of " << *this << std::endl;
    inferrance_t inf = infer(cont);
    std::cout << "Type of " << *this << " inferred as " << *inf.typ << " compared to " << *typ << std::endl;
    if (typ->conv(cont.types.size(),inf.typ)) {
        std::cout << "Type successfully inferred" << std::endl;
        return inf.term;
    }
    else {
        std::stringstream ss(""); 
        ss << "type mismatch between\n" << *typ << "\nand\n" << *inf.typ << std::endl;
        ss << *this;
        ss << cont;
        throw ss.str();
    }
}
term_ptr rabs_t::check(context_t& cont,value_ptr v) {
    return v->check_RABS(cont,var,body);
}
term_ptr rlet_t::check(context_t& cont,value_ptr v) {
    std::cout << "Checking type of let " << *typ << std::endl;
    term_ptr ta = typ->check(cont,VU);
    std::cout << "Evaluating type of let " << *ta << std::endl;
    value_ptr va = ta->eval(cont.environment);
    std::cout << "Checking definition of let " << *def << " of type " << *va << std::endl;
    term_ptr tt = def->check(cont,va);
    value_ptr vt = tt->eval(cont.environment);
    cont.new_val(var,va,vt);
    std::cout << "Checking body of let " << *body << " of type " << *v << std::endl;
    term_ptr tu = body->check(cont,v);
    return std::make_shared<let_t>(var,ta,tt,tu);
}

inferrance_t raw_t::infer(context_t&){
    throw "Inferring an unknown raw term.";
}
inferrance_t rabs_t::infer(context_t&){
    throw "Cannot infer the type of a Lambda.";
}
inferrance_t ru_t::infer(context_t&){
    return inferrance_t(std::make_shared<u_t>(), VU);
}
inferrance_t rvar_t::infer(context_t& cont){
    try {
        auto res = cont.types.at(name);
        term_ptr term = std::make_shared<var_t>(cont.level-1-res.second);
        return inferrance_t(term,res.first);
    }
    catch (const std::out_of_range& e) {
        throw "Unbounded variable"+name;
    }
}
inferrance_t rapp_t::infer(context_t& cont) {
    inferrance_t inf = left->infer(cont);
    return inf.typ->infer_RAPP(cont,inf.term,right);
}
inferrance_t rpi_t::infer(context_t& cont) {
    std::cout << "Checking left pi type " << *typ << std::endl;
    std::cout << cont;
    term_ptr ttyp = typ->check(cont,VU);
    std::cout << "Evaluating and binding left pi type " << *ttyp << std::endl;
    cont.new_var(var,ttyp->eval(cont.environment));
    std::cout << "Checking right pi type " << *body << std::endl;
    term_ptr tbody = body->check(cont,VU);
    cont.pop(var);
    return inferrance_t(std::make_shared<pi_t>(var,ttyp,tbody),VU);
}
inferrance_t rlet_t::infer(context_t& cont) {
    term_ptr ttyp = typ->check(cont,VU);
    value_ptr vtyp = ttyp->eval(cont.environment);
    term_ptr tdef = def->check(cont,vtyp);
    value_ptr vdef = tdef->eval(cont.environment);
    cont.new_val(var,vtyp,vdef);
    inferrance_t inf = body->infer(cont);
    cont.pop(var);
    return inferrance_t(std::make_shared<let_t>(var,ttyp,tdef,inf.term),inf.typ);
}

