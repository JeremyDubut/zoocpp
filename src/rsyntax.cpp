#include <iostream>
#include <sstream>
#include "value.hpp"
#include "rsyntax.hpp"
#include "syntax.hpp"

#define BIND_CONTEXT(v,b,bp) \
    environment.push_back(v); \
    types.try_emplace(var,std::vector<type_t>()); \
    type_t typ{t,level,bp}; \
    types[var].push_back(typ); \
    flags.push_back(b); \
    level++;

std::ostream& operator<< (std::ostream& out, raw_t& term) {return term.to_string(out);}


std::ostream& raw_t::to_string(std::ostream& out) {return out << "Unknown raw term";}
std::ostream& rvar_t::to_string(std::ostream& out) {return out << name;}
std::ostream& rabs_t::to_string(std::ostream& out) {return out << "λ" << var << " " << *body;}
std::ostream& riabs_t::to_string(std::ostream& out) {return out << "λ{" << var << "} " << *body;}
std::ostream& rnabs_t::to_string(std::ostream& out) {return out << "λ{" << var << "=" << ivar << "} " << *body;}
std::ostream& rapp_t::to_string(std::ostream& out) {return out << "(" << *left << " " << *right << ")";}
std::ostream& riapp_t::to_string(std::ostream& out) {return out << "(" << *left << " {" << *right << "})";}
std::ostream& rnapp_t::to_string(std::ostream& out) {return out << "(" << *left << " {" << ivar << "=" << *right << "})";}
std::ostream& rlet_t::to_string(std::ostream& out) {
    return out << "Let " << var << " : " << *typ << " = " << *def << " in" << std::endl << *body;
}
std::ostream& ru_t::to_string(std::ostream& out) {return out << "𝒰";}
std::ostream& rpi_t::to_string(std::ostream& out) {return out << "(" << var << " : " << *typ << ") → " << *body;}
std::ostream& ripi_t::to_string(std::ostream& out) {return out << "{" << var << " : " << *typ << "} → " << *body;}
std::ostream& rhole_t::to_string(std::ostream& out) {return out << "?_";}
std::ostream& rnl_t::to_string(std::ostream& out) {return out << std::endl << *body;}

raw_ptr raw_t::update_body(raw_ptr body) {return body;}
raw_ptr rabs_t::update_body(raw_ptr body) {
    this->body = this->body->update_body(body);
    return shared_from_this();
}

name_t raw_t::get_name() const {return "Unknown name";}
name_t rvar_t::get_name() const {return name;}
name_t rhole_t::get_name() const {return "_";}

void context_t::new_var(name_t var, value_ptr t) {
    BIND_CONTEXT(std::make_shared<vrig_t>(level),true,true);
}
void context_t::new_val(name_t var, value_ptr t, value_ptr v) {
    BIND_CONTEXT(v,false,true);
}
void context_t::new_bind(name_t var, value_ptr t) {
    BIND_CONTEXT(std::make_shared<vrig_t>(level),true,false);
}
void context_t::pop(name_t var) {
    environment.pop_back();
    level = level-1;
    types[var].pop_back();
    if (types[var].empty()) {
        types.erase(var);
    }
    flags.pop_back();
}

std::ostream& operator<< (std::ostream& out, context_t& cont) {
    out << std::endl << "Context" << std::endl;
    out << "- Environment" << std::endl;
    out << cont.environment << std::endl;
    out << "- Level: " << cont.level << std::endl;
    // out << "- Types" << std::endl; TODO
    // for (auto it = cont.types.begin(); it != cont.types.end(); it++) {
    //     out << it->first << " " << *((it->second).first) << " " << (it->second).second << std::endl;
    // }
    return out;
}

term_ptr raw_t::check(context_t& cont,value_ptr typ) {
    // std::cout << "Checking term " << *this << " with type " << *typ << std::endl;
    inferrance_t inf = infer(cont);
    typ->unify(cont.level,inf.typ);
    // std::cout << "Type checking of term " << *this << " with type " << *typ << " successful, inferred as " << *inf.term << std::endl;
    return inf.term;
    // if (typ->conv(cont.level,inf.typ)) {
    //     // // std::cout << "Type checking of term " << *this << " with type " << *typ << " successful, inferred as " << *inf.term << std::endl; 
    //     return inf.term;
    // }
    // else {
    //     std::stringstream ss(""); 
    //     ss << "type mismatch between\n" << *typ->quote(0)->display() << "\nand\n" << *inf.typ->quote(0)->display() << std::endl;
    //     ss << *this;
    //     ss << cont;
    //     throw ss.str();
    // }
}
term_ptr rabs_t::check(context_t& cont,value_ptr v) {
    // std::cout << "Checking Lam " << *this << " with type " << *v << std::endl;
    term_ptr res = v->check_RABS(cont,var,body);
    // std::cout << "Type check of Lam " << *this << " with type " << *v << " successful, inferred as " << *res << std::endl;
    return res;
}
term_ptr riabs_t::check(context_t& cont,value_ptr v) {
    // std::cout << "Checking Lam " << *this << " with type " << *v << std::endl;
    term_ptr res = v->check_RIABS(cont,var,body);
    // std::cout << "Type check of Lam " << *this << " with type " << *v << " successful, inferred as " << *res << std::endl;
    return res;
}
term_ptr rnabs_t::check(context_t& cont,value_ptr v) {
    // std::cout << "Checking Lam " << *this << " with type " << *v << std::endl;
    term_ptr res = v->check_RNABS(cont,var,ivar,body);
    // std::cout << "Type check of Lam " << *this << " with type " << *v << " successful, inferred as " << *res << std::endl;
    return res;
}
term_ptr rlet_t::check(context_t& cont,value_ptr v) {
    // std::cout << "Checking Let " << *typ << " with type " << *v << std::endl;
    term_ptr res = v->check_LET(cont,var,typ,def,body);
    return res;
    // std::cout << "Type check of Let " << *this << " with type " << *v << " successful, inferred as " << *res << std::endl;
    return res;
}
term_ptr rhole_t::check(context_t& cont,value_ptr v) {
    // std::cout << "Checking Hole" << std::endl;
    term_ptr res = v->check_HOLE(cont);
    // std::cout << "Type check of hole done" << std::endl;
    return res;
}

inferrance_t raw_t::infer(context_t&){
    throw "Inferring an unknown raw term.";
}
inferrance_t rabs_t::infer(context_t& cont){
    // std::cout << "Inferring Lam" << *this << std::endl;
    value_ptr a = FRESHMETA->eval(cont.environment);
    cont.new_var(var,a);
    inferrance_t inf = body->infer(cont);
    inf = inf.term->insert(cont,inf.typ);
    cont.pop(var);
    closure_t clos = closure_t(cont.environment,inf.typ->quote(cont.level+1));
    inferrance_t res = inferrance_t(std::make_shared<abs_t>(var,inf.term),std::make_shared<vpi_t>(var,a,clos));
    // std::cout << "Inferrance of Lam" << *this << "successful, inferred as " << *res.term << " with type " << *res.typ << std::endl;
    return res;
}
inferrance_t rnabs_t::infer(context_t&) {
    throw "Cannot infer a named implicit lambda";
}
inferrance_t ru_t::infer(context_t&){
    // std::cout << "Inferring U" << std::endl;
    // std::cout << "Inferrance of U done" << std::endl;
    return inferrance_t(std::make_shared<u_t>(), VU);
}
inferrance_t rvar_t::infer(context_t& cont){
    try {
        // std::cout << "Inferring variable " << name << std::endl;
        auto res = cont.types.at(name).end()-1;
        while (res != cont.types.at(name).begin()-1 && !res->source) {
            res = res-1;
        }
        if (res != cont.types.at(name).begin()-1) {
            term_ptr term = std::make_shared<var_t>(cont.level-1-res->level);
            // std::cout << "Variable  " << name << " inferred as " << *term << " of typ " << *res.first << std::endl;
            return inferrance_t(term,res->typ->clone());
        }
        else {
            throw "Unbounded variable "+name;
        }
    }
    catch (const std::out_of_range& e) {
        throw "Unbounded variable "+name;
    }
}
inferrance_t rapp_t::infer(context_t& cont) {
    // std::cout << "Inferring app " << *this->left << " and " << *this->right << std::endl;
    inferrance_t inff = left->infer(cont);
    inff = inff.typ->force()->insert(cont,inff.term);
    std::pair<value_ptr,closure_t> infr = inff.typ->force()->infer_RAPP(cont);
    term_ptr rterm = right->check(cont,infr.first);
    CAPP(rterm->eval(cont.environment),infr.second,typ)
    // std::cout << "App " << *this << " inferred as " << *res.term << " of type " << *res.typ << std::endl;
    return inferrance_t(std::make_shared<app_t>(inff.term,rterm),typ);
}
inferrance_t riapp_t::infer(context_t& cont) {
    // std::cout << "Inferring app " << *this->left << " and " << *this->right << std::endl;
    inferrance_t inff = left->infer(cont);
    std::pair<value_ptr,closure_t> infr = inff.typ->force()->infer_RINAPP(cont);
    term_ptr rterm = right->check(cont,infr.first);
    CAPP(rterm->eval(cont.environment),infr.second,typ)
    // std::cout << "App " << *this << " inferred as " << *res.term << " of type " << *res.typ << std::endl;
    return inferrance_t(std::make_shared<iapp_t>(inff.term,rterm),typ);
}
inferrance_t rnapp_t::infer(context_t& cont) {
    // std::cout << "Inferring app " << *this->left << " and " << *this->right << std::endl;
    inferrance_t inff = left->infer(cont);
    inff = inff.typ->force()->insertUntilName(cont,ivar,inff.term);
    std::pair<value_ptr,closure_t> infr = inff.typ->force()->infer_RINAPP(cont);
    term_ptr rterm = right->check(cont,infr.first);
    CAPP(rterm->eval(cont.environment),infr.second,typ)
    // std::cout << "App " << *this << " inferred as " << *res.term << " of type " << *res.typ << std::endl;
    return inferrance_t(std::make_shared<iapp_t>(inff.term,rterm),typ);
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
inferrance_t ripi_t::infer(context_t& cont) {
    // std::cout << "Inferring pi " << *this << std::endl;
    term_ptr ttyp = typ->check(cont,VU);
    cont.new_var(var,ttyp->eval(cont.environment));
    term_ptr tbody = body->check(cont,VU);
    cont.pop(var);
    term_ptr res = std::make_shared<ipi_t>(var,ttyp,tbody);
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
inferrance_t rhole_t::infer(context_t& cont) {
    // std::cout << "Inferring hole" << std::endl;
    value_ptr a = FRESHMETA->eval(cont.environment);
    // std::cout << "Inferrance of hole successfule, inferred with type " << *a << std::endl;
    return inferrance_t(FRESHMETA,a);
}


raw_ptr raw_t::build(raw_ptr) {throw "Cannot build an non-utility raw type";}
raw_ptr iicit::build(raw_ptr r) {return std::make_shared<riabs_t>(bind,r);}
raw_ptr eicit::build(raw_ptr r) {return std::make_shared<rabs_t>(bind,r);}
raw_ptr nicit::build(raw_ptr r) {return std::make_shared<rnabs_t>(bind,r,name);}
raw_ptr iarg_t::build(raw_ptr r) {return std::make_shared<riapp_t>(r,arg);}
raw_ptr earg_t::build(raw_ptr r) {return std::make_shared<rapp_t>(r,arg);}
raw_ptr narg_t::build(raw_ptr r) {return std::make_shared<rnapp_t>(r,arg,name);}
#define BUILDPI(t) \
    while (!names.empty()) { \
        name_t var = *(names.end()-1); \
        names.pop_back(); \
        res = std::make_shared<t>(var,typ,res); \
    } 
raw_ptr pibinder_t::build(raw_ptr r) {
    raw_ptr res = r;
    if (icit) {
        BUILDPI(ripi_t);
    }
    else {
        BUILDPI(rpi_t);
    }
    return res;
}
raw_ptr pibinderlist_t::build(raw_ptr r) {
    raw_ptr res = r;
    while (!binders.empty()) {
        raw_ptr bind = *(binders.end()-1);
        binders.pop_back();
        res = bind->build(res);
    }
    return res;
}
raw_ptr icitlist_t::build(raw_ptr r) {
    raw_ptr res = r;
    while (!icits.empty()) {
        raw_ptr icit = *(icits.end()-1);
        icits.pop_back();
        res = icit->build(res);
    }
    return res;
}
raw_ptr arglist_t::build(raw_ptr r) {
    raw_ptr res = r;
    for (raw_ptr arg : args) {
        res = arg->build(res);
    }
    return res;
}
raw_ptr raw_t::auto_build() {throw "Cannot auto build a non-list of arguments";}
raw_ptr arglist_t::auto_build() {
    raw_ptr res = args[0];
    for (auto it = args.begin()+1; it!=args.end(); it++) {
        res = (*it)->build(res);
    }
    return res;
}
void raw_t::push_back(raw_ptr) {throw "Cannot push_back on a non-list type";}
void pibinderlist_t::push_back(raw_ptr r) {binders.push_back(r);}
void arglist_t::push_back(raw_ptr r) {args.push_back(r);}
void icitlist_t::push_back(raw_ptr r) {icits.push_back(r);}
std::vector<name_t>* raw_t::get_namelist() { throw "Not a name list"; }
std::vector<name_t>* namelist_t::get_namelist() { return names; }
// std::vector<name_t> raw_t::get_names() const { throw "Not a binder"; }
// std::vector<raw_ptr>* raw_t::get_binderlist() const { throw "Not a binder list"; }
// raw_ptr raw_t::build_icit(raw_ptr) const { throw "Not an icit"; }
// std::vector<raw_ptr>* raw_t::get_icitlist() const { throw "Not a icit list"; }
// arg_t raw_t::build_arg(raw_ptr) const { throw "Not an arg"; }
// raw_ptr raw_t::build_pi(raw_ptr) const { throw "Not an arg"; }
// std::vector<raw_ptr>* raw_t::get_arglist() const { throw "Not an arg list"; }
// std::vector<name_t> pibinder_t::get_names() const {return names;}
// raw_ptr pibinder_t::build_pi(raw_ptr r){
//     raw_ptr res = r;
//     while (!names.empty()) {
//         name_t var = names.end()-1;
//     }
// }

// raw_ptr pifoldr(std::vector<raw_ptr>* v, raw_ptr r) {
//     if (v->empty()) {
//         return r;
//     }
//     raw_ptr pb = *(v->end()-1);
//     std::vector<name_t> names = pb->get_names();
//     v->pop_back();
//     raw_ptr res = pifoldr(v,r);
//     res = pb.build_pi(res);
//     while (!names.empty()) {
//         name_t var = *(names.end()-1);
//         names.pop_back();
//         if (pb.icit) {
//             res = std::make_shared<ripi_t>(var,pb.typ,res);
//         }
//         else {
//             res = std::make_shared<rpi_t>(var,pb.typ,res);
//         }
//     }
//     return res;
// }
// raw_ptr lamfoldr(std::vector<icit>* v, raw_ptr r) {
//     raw_ptr res = r;
//     while (!v->empty()) {
//         icit d = *(v->end()-1);
//         res = d.rptr(res);
//     }
//     return res;
// }
// raw_ptr appfoldl(std::vector<arg_t>* v1, std::vector<arg_t>* v2) {
//     auto it = v1->begin();
//     raw_ptr res = it->arg;
//     it++;
//     while (it != v1->end()) {
//         res = it->rptr(res);
//         it++;
//     }
//     for (arg_t arg : *v2) {
//         res = arg.rptr(res);
//     }
//     return res;
// }