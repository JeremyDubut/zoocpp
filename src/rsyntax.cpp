#include <iostream>
#include <sstream>
#include "value.hpp"
#include "rsyntax.hpp"
#include "syntax.hpp"

#define BIND_CONTEXT(v) \
    environment.push_back(v); \
    types[var] = std::pair<value_ptr,std::size_t>(t,level); \
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
    return body->to_string(out << " in");
}
std::ostream& ru_t::to_string(std::ostream& out) {
    return out << "𝒰";
}
std::ostream& rpi_t::to_string(std::ostream& out) {
    typ->to_string(out << "(" << var << " : ");
    return body->to_string(out << ") → ");
}
std::ostream& rhole_t::to_string(std::ostream& out) {return out << "?_";}
std::ostream& rnl_t::to_string(std::ostream& out) {return out << std::endl << *body;}

raw_ptr raw_t::update_body(raw_ptr body) {return body;}
raw_ptr rabs_t::update_body(raw_ptr body) {
    this->body = this->body->update_body(body);
    return shared_from_this();
}

name_t raw_t::get_name() {return "Unknown name";}
name_t rvar_t::get_name() {return name;}

void context_t::new_var(name_t var, value_ptr t) {
    BIND_CONTEXT(std::make_shared<vvar_t>(level));
}
void context_t::new_val(name_t var, value_ptr t, value_ptr v) {
    BIND_CONTEXT(v);
}
void context_t::pop(name_t var) {
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
    // std::cout << "Checking term " << *this << " with type " << *typ << std::endl;
    inferrance_t inf = infer(cont);
    if (typ->conv(cont.level,inf.typ)) {
        // std::cout << "Type checking of term " << *this << " with type " << *typ << " successful, inferred as " << *inf.term << std::endl; 
        return inf.term;
    }
    else {
        std::stringstream ss(""); 
        ss << "type mismatch between\n" << *typ->quote(0)->display() << "\nand\n" << *inf.typ->quote(0)->display() << std::endl;
        ss << *this;
        ss << cont;
        throw ss.str();
    }
}
term_ptr rabs_t::check(context_t& cont,value_ptr v) {
    // std::cout << "Checking Lam " << *this << " with type " << *v << std::endl;
    term_ptr res = v->check_RABS(cont,var,body);
    // std::cout << "Type check of Lam " << *this << " with type " << *v << " successful, inferred as " << *res << std::endl;
    return res;
}
term_ptr rlet_t::check(context_t& cont,value_ptr v) {
    // std::cout << "Checking Let " << *typ << " with type " << *v << std::endl;
    term_ptr ta = typ->check(cont,VU);
    value_ptr va = ta->eval(cont.environment);
    term_ptr tt = def->check(cont,va);
    value_ptr vt = tt->eval(cont.environment);
    cont.new_val(var,va,vt);
    term_ptr tu = body->check(cont,v);
    term_ptr res = std::make_shared<let_t>(var,ta,tt,tu);
    // std::cout << "Type check of Let " << *this << " with type " << *v << " successful, inferred as " << *res << std::endl;
    return res;
}

inferrance_t raw_t::infer(context_t&){
    throw "Inferring an unknown raw term.";
}
inferrance_t rabs_t::infer(context_t&){
    throw "Cannot infer the type of a Lambda.";
}
inferrance_t ru_t::infer(context_t&){
    // std::cout << "Inferring U" << std::endl;
    return inferrance_t(std::make_shared<u_t>(), VU);
}
inferrance_t rvar_t::infer(context_t& cont){
    try {
        // std::cout << "Inferring variable " << name << std::endl;
        auto res = cont.types.at(name);
        term_ptr term = std::make_shared<var_t>(cont.level-1-res.second);
        // std::cout << "Variable  " << name << " inferred as " << *term << " of typ " << *res.first << std::endl;
        return inferrance_t(term,res.first);
    }
    catch (const std::out_of_range& e) {
        throw "Unbounded variable "+name;
    }
}
inferrance_t rapp_t::infer(context_t& cont) {
    // std::cout << "Inferring app " << *this << std::endl;
    inferrance_t inf = left->infer(cont);
    inferrance_t res = inf.typ->infer_RAPP(cont,inf.term,right);
    // std::cout << "App " << *this << " inferred as " << *res.term << " of type " << *res.typ << std::endl;
    return res;
}
inferrance_t rpi_t::infer(context_t& cont) {
    // std::cout << "Inferring pi " << *this << std::endl;
    term_ptr ttyp = typ->check(cont,VU);
    cont.new_var(var,ttyp->eval(cont.environment));
    term_ptr tbody = body->check(cont,VU);
    cont.pop(var);
    term_ptr res = std::make_shared<pi_t>(var,ttyp,tbody);
    // std::cout << "Pi " << *this << " inferred as " << *res << " of type U" << std::endl;
    return inferrance_t(res,VU);
}
inferrance_t rlet_t::infer(context_t& cont) {
    // std::cout << "Inferring let " << *this << std::endl;
    term_ptr ttyp = typ->check(cont,VU);
    value_ptr vtyp = ttyp->eval(cont.environment);
    term_ptr tdef = def->check(cont,vtyp);
    value_ptr vdef = tdef->eval(cont.environment);
    cont.new_val(var,vtyp,vdef);
    inferrance_t inf = body->infer(cont);
    cont.pop(var);
    inferrance_t res = inferrance_t(std::make_shared<let_t>(var,ttyp,tdef,inf.term),inf.typ);
    // std::cout << "Let " << *this << " inferred as " << *res.term << " of type " << *res.typ << std::endl;
    return res;
}

