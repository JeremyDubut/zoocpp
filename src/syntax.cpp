#include "syntax.hpp"
#include "value.hpp"
#include "rsyntax.hpp"

#define VMETA(n) \
    metavar_t::lookup(n)->get_value(n)->clone()

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
std::ostream& appp_t::to_string(std::ostream& out) {return out << "(" << *left << " [prunes])";}
std::ostream& tcheck_t::to_string(std::ostream& out) {return out << "!" << index;}
std::ostream& operator<< (std::ostream& out, term_t& term) {return term.to_string(out);}

value_ptr term_t::eval() {
    environment_t env {};
    return eval(env);
}
value_ptr term_t::eval(environment_t&) {
    throw "Evaluation of an unknown term.";
}
value_ptr var_t::eval(environment_t& env) {
    // LOG("Evaluating variable " << *this);
    return env.at(env.size()-1-index)->clone();
}
value_ptr abs_t::eval(environment_t& env) {
    // LOG("Evaluating explicit lambda " << *this);
    return std::make_shared<vabs_t>(var,env,body);
}
value_ptr iabs_t::eval(environment_t& env) {
    // LOG("Evaluating implicit lambda " << *this);
    return std::make_shared<viabs_t>(var,env,body);
}
value_ptr let_t::eval(environment_t& env) {
    // LOG("Evaluating let ");
    env.push_back(def->eval(env));
    return body->eval(env);
}
value_ptr app_t::eval(environment_t& env) {
    // LOG("Evaluating explicit application " << *this);
    return left->eval(env)->vApp(right->eval(env),false);
}
value_ptr iapp_t::eval(environment_t& env) {
    // LOG("Evaluating implicit application " << *this);
    return left->eval(env)->vApp(right->eval(env),true);
}
value_ptr u_t::eval(environment_t&) {
    return std::make_shared<vu_t>();
}
value_ptr pi_t::eval(environment_t& env) {
    // LOG("Evaluating explicit pi " << *this);
    return std::make_shared<vpi_t>(var,typ->eval(env),env,body);
}
value_ptr ipi_t::eval(environment_t& env) {
    // LOG("Evaluating implicit pi " << *this);
    return std::make_shared<vipi_t>(var,typ->eval(env),env,body);
}
value_ptr meta_t::eval(environment_t&) {
    // LOG("Evaluating metavariable " << *this << " with value " << *VMETA(index));
    return VMETA(index);
}
value_ptr appp_t::eval(environment_t& env) {
    // LOG("Evaluating application/pruning " << *this);
    if (env.size() != prune.size()) {
        std::stringstream ss("");
        ss << "Evaluation error: Inconsistency between environments " << env.size() << " and prunings " << prune.size();
        throw ss.str();
    }
    else {
        auto ite = env.begin();
        value_ptr res = left->eval(env);
        for (pruning_t itf : prune) {
            switch (itf) {
                case Implicit: res = res->vApp(*ite,true); break;
                case Explicit: res = res->vApp(*ite,false); break;
                case None: break;
            }
            ite++;
        }
        return res;
    }
}
value_ptr tcheck_t::eval(environment_t& env) {
    // LOG("Evaluating check " << *this);
    return check_t::lookup(index)->read()->eval(env); // TODO: avoid copying of pruning
}

term_ptr term_t::nf() {
    environment_t env {};
    return eval(env)->quote(0);
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

term_ptr locals_t::closety(term_ptr body) {
    return body;
}
term_ptr lbind_t::closety(term_ptr body) {
    return mcl->closety(std::make_shared<pi_t>(var,typ,body));
}
term_ptr ldefine_t::closety(term_ptr body) {
    return mcl->closety(std::make_shared<let_t>(var,typ,def,body));
}
term_ptr locals_t::closetm(term_ptr body) {
    return body;
}
term_ptr lbind_t::closetm(term_ptr body) {
    return mcl->closetm(std::make_shared<abs_t>(var,body));
}
term_ptr ldefine_t::closetm(term_ptr body) {
    return mcl->closetm(std::make_shared<let_t>(var,typ,def,body));
}

locals_ptr locals_t::pop() {
    throw "Cannot pop a empty local";
}
locals_ptr lbind_t::pop() {
    return mcl;
}
locals_ptr ldefine_t::pop() {
    return mcl;
}
