#include <iostream>
#include <sstream>
#include "syntax.hpp"
#include "value.hpp"
#include "rsyntax.hpp"

#define VMETA(n) \
    metavar_t::lookup(n).value_or(std::make_shared<vflex_t>(n))->clone()

std::ostream& term_t::to_string(std::ostream& out) {return out << "Unknown term";}
std::ostream& var_t::to_string(std::ostream& out) {return out << "Var" << index;}
std::ostream& abs_t::to_string(std::ostream& out) {return out << "λ" << var << " " << *body;}
std::ostream& iabs_t::to_string(std::ostream& out) {return out << "λ{" << var << "} " << *body;}
std::ostream& app_t::to_string(std::ostream& out) {return out << "(" << *left << " " << *right << ")";}
std::ostream& iapp_t::to_string(std::ostream& out) {return out << "(" << *left << " {" << *right << "})";}
std::ostream& let_t::to_string(std::ostream& out) {return out << "Let " << *def << " in" << std::endl << *body;}
std::ostream& u_t::to_string(std::ostream& out) {return out << "𝒰";}
std::ostream& pi_t::to_string(std::ostream& out) {return out << "(" << var << " : " << *typ << ") → " << *body;}
std::ostream& ipi_t::to_string(std::ostream& out) {return out << "{" << var << " : " << *typ << "} → " << *body;}
std::ostream& meta_t::to_string(std::ostream& out) {return out << "?" << index;}
std::ostream& imeta_t::to_string(std::ostream& out) {return out << "?" << index;}
std::ostream& operator<< (std::ostream& out, term_t& term) {return term.to_string(out);}

value_ptr term_t::eval(environment_t&) {
    throw "Evaluation of an unknown term.";
}
value_ptr var_t::eval(environment_t& env) {
    return env.at(env.size()-1-index)->clone();
}
value_ptr abs_t::eval(environment_t& env) {
    return std::make_shared<vabs_t>(var,env,body);
}
value_ptr iabs_t::eval(environment_t& env) {
    return std::make_shared<viabs_t>(var,env,body);
}
value_ptr let_t::eval(environment_t& env) {
    env.push_back(def->eval(env));
    return body->eval(env);
}
value_ptr app_t::eval(environment_t& env) {
    return left->eval(env)->vApp(right->eval(env),false);
    // return std::make_shared<vapp_t>(*left.eval(env),*right.eval(env));
}
value_ptr iapp_t::eval(environment_t& env) {
    return left->eval(env)->vApp(right->eval(env),true);
    // return std::make_shared<vapp_t>(*left.eval(env),*right.eval(env));
}
value_ptr u_t::eval(environment_t&) {
    return std::make_shared<vu_t>();
}
value_ptr pi_t::eval(environment_t& env) {
    return std::make_shared<vpi_t>(var,typ->eval(env),env,body);
}
value_ptr ipi_t::eval(environment_t& env) {
    return std::make_shared<vipi_t>(var,typ->eval(env),env,body);
}
value_ptr meta_t::eval(environment_t&) {
    return VMETA(index);
}
value_ptr imeta_t::eval(environment_t& env) {
    if (env.size() != flags.size()) {
        std::stringstream ss("");
        ss << "Inconsistency between environments " << env << " and flags " << flags.size();
        throw ss.str();
    }
    else {
        auto ite = env.begin();
        value_ptr res = VMETA(index);
        // std::cout << "Looking out " << index << " with " << *res << " and " << flags.size() << std::endl;
        for (bool itf : flags) {
            // std::cout << itf << std::endl;
            if (itf) {
                res = res->vApp((*ite)->clone(),false);
            }
            ite++;
        }
        return res;
    }
}

term_ptr term_t::nf(environment_t& env) {
    std::size_t length = env.size();
    return eval(env)->quote(length);
}

raw_ptr term_t::display() {
    names_t names = names_t();
    return display_rec(names);}
raw_ptr term_t::display_rec(names_t&) {
    throw "Display unknown value";
}
raw_ptr var_t::display_rec(names_t& names) {
    return std::make_shared<rvar_t>(names[names.size()-1-index]);
}
raw_ptr abs_t::display_rec(names_t& names) {
    names.push_back(var);
    raw_ptr res = std::make_shared<rabs_t>(var,body->display_rec(names));
    names.pop_back();
    return res;
}
raw_ptr iabs_t::display_rec(names_t& names) {
    names.push_back(var);
    raw_ptr res = std::make_shared<riabs_t>(var,body->display_rec(names));
    names.pop_back();
    return res;
}
raw_ptr app_t::display_rec(names_t& names) {
    return std::make_shared<rapp_t>(left->display_rec(names),right->display_rec(names));
}
raw_ptr iapp_t::display_rec(names_t& names) {
    return std::make_shared<riapp_t>(left->display_rec(names),right->display_rec(names));
}
raw_ptr let_t::display_rec(names_t& names) {
    raw_ptr rtyp = typ->display_rec(names);
    raw_ptr rdef = def->display_rec(names);
    names.push_back(var);
    raw_ptr rbody = body->display_rec(names);
    names.pop_back();
    return std::make_shared<rlet_t>(var,rtyp,rdef,rbody);
}
raw_ptr u_t::display_rec(names_t&) {
    return std::make_shared<ru_t>();
}
raw_ptr pi_t::display_rec(names_t& names) {
    raw_ptr rtyp = typ->display_rec(names);
    names.push_back(var);
    raw_ptr rbody = body->display_rec(names);
    names.pop_back();
    return std::make_shared<rpi_t>(var,rtyp,rbody);
}
raw_ptr ipi_t::display_rec(names_t& names) {
    raw_ptr rtyp = typ->display_rec(names);
    names.push_back(var);
    raw_ptr rbody = body->display_rec(names);
    names.pop_back();
    return std::make_shared<ripi_t>(var,rtyp,rbody);
}

inferrance_t term_t::insert(context_t& cont, value_ptr typ) {
    return typ->force()->insert(cont,shared_from_this());
}
inferrance_t iabs_t::insert(context_t&, value_ptr typ) {
    return inferrance_t(shared_from_this(),typ);
}