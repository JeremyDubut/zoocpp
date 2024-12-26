#include "value.hpp"
#include "rsyntax.hpp"
#include "syntax.hpp"

std::ostream& operator<< (std::ostream& out, raw_t& term) {return term.to_string(out);}
std::ostream& raw_t::to_string(std::ostream& out) {return out << "Unknown raw term";}
std::ostream& rvar_t::to_string(std::ostream& out) {return out << name;}
std::ostream& rtabs_t::to_string(std::ostream& out) {return out << "λ(" << var << ": " << *typ << ") " << *body;}
std::ostream& rabs_t::to_string(std::ostream& out) {return out << "λ" << var << " " << *body;}
std::ostream& riabs_t::to_string(std::ostream& out) {return out << "λ{" << var << "} " << *body;}
std::ostream& rtiabs_t::to_string(std::ostream& out) {return out << "λ{" << var << ": " << *typ << "} " << *body;}
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

name_t raw_t::get_name() const {return "Unknown name";}
name_t rvar_t::get_name() const {return name;}
name_t rhole_t::get_name() const {return "_";}

void context_t::new_var(name_t var, value_ptr t) {
    environment.push_back(std::make_shared<vrig_t>(level));
    local = std::make_unique<lbind_t>(local,var,t->quote(level));
    prune.push_back(Explicit);
    type_t typ{t,level}; 
    types[var].push_back(typ); 
    level++;
}
void context_t::new_val(name_t var, term_ptr ttyp, value_ptr vtyp, term_ptr tdef, value_ptr vdef) { 
    environment.push_back(vdef);
    local = std::make_unique<ldefine_t>(local,var,ttyp,tdef); 
    prune.push_back(None);
    type_t typ{vtyp,level}; 
    types[var].push_back(typ); 
    level++;
}
void context_t::new_bind(name_t var, value_ptr t) {
    environment.push_back(std::make_shared<vrig_t>(level));
    local = std::make_unique<lbind_t>(local,var,t->quote(level));
    prune.push_back(Explicit);
    level++;
}
void context_t::pop(name_t var) {
    environment.pop_back();
    level = level-1;
    types[var].pop_back();
    if (types[var].empty()) {
        types.erase(var);
    }
    prune.pop_back();
    local = std::move(*local->pop());
}
void context_t::pop() {
    environment.pop_back();
    level = level-1;
    prune.pop_back();
    local = std::move(*local->pop());
}

// Types take too long to print
// removed them for now
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
    LOG("Checking term " << *this << " with type " << *typ);
    term_ptr res = typ->force()->check_RAW(cont,shared_from_this());
    LOG("Type checking of term " << *this << " with type " << *typ << " successful");
    return res;
}
term_ptr rabs_t::check(context_t& cont,value_ptr v) {
    LOG("Checking Explicit Lam " << *this << " with type " << *v);
    term_ptr res = v->force()->check_RABS(cont,var,body);
    LOG("Type check of Explicit Lam " << *this << " with type " << *v << " successful");
    return res;
}
term_ptr rtabs_t::check(context_t& cont,value_ptr v) {
    LOG("Checking Typed Explicit Lam " << *this << " with type " << *v);
    term_ptr res = v->force()->check_RTABS(cont,var,typ,body);
    LOG("Type check of Typed Explicit Lam " << *this << " with type " << *v << " successful");
    return res;
}
term_ptr rtiabs_t::check(context_t& cont,value_ptr v) {
    LOG("Checking Typed Implicit Lam " << *this << " with type " << *v);
    term_ptr res = v->force()->check_RTIABS(cont,var,typ,body);
    LOG("Type check of Typed Implicit Lam " << *this << " with type " << *v << " successful");
    return res;
}
term_ptr riabs_t::check(context_t& cont,value_ptr v) {
    LOG("Checking Implicit Lam " << *this << " with type " << *v);
    term_ptr res = v->force()->check_RIABS(cont,var,body);
    LOG("Type check of Implicit Lam " << *this << " with type " << *v << " successful");
    return res;
}
term_ptr rnabs_t::check(context_t& cont,value_ptr v) {
    LOG("Checking Named Implicit Lam " << *this << " with type " << *v);
    term_ptr res = v->force()->check_RNABS(cont,var,ivar,body);
    LOG("Type check of Named Implicit Lam " << *this << " with type " << *v << " successful");
    return res;
}
term_ptr rlet_t::check(context_t& cont,value_ptr v) {
    LOG("Checking Let " << var << " : " << *typ << " = " << *def << " in [...] with type " << *v);
    term_ptr res = v->force()->check_LET(cont,var,typ,def,body);
    LOG("Type check of Let " << var << " : " << *typ << " = " << *def << " in [...] with type " << *v << " successful");
    return res;
}
term_ptr rhole_t::check(context_t& cont,value_ptr v) {
    LOG("Checking Hole");
    term_ptr res = v->force()->check_HOLE(cont);
    LOG("Type check of hole successful");
    return res;
}

inferrance_t raw_t::infer(context_t&){
    throw "Inferrance error: Inferring an unknown raw term.";
}
inferrance_t rabs_t::infer(context_t& cont){
    LOG("Inferring Explicit Lam" << *this);
    value_ptr a = FRESHMETA(VU)->eval(cont.environment); // Type of FM
    cont.new_var(var,a);
    inferrance_t inf = body->infer(cont);
    inf = inf.term->insert(cont,inf.typ);
    cont.pop(var);
    closure_t clos = closure_t(cont.environment,inf.typ->quote(cont.level+1));
    inferrance_t res = inferrance_t(std::make_shared<abs_t>(var,inf.term),std::make_shared<vpi_t>(var,a,clos));
    LOG("Inferrance of Explicit Lam" << *this << "successful, inferred with type " << *res.typ);
    return res;
}
inferrance_t rnabs_t::infer(context_t&) {
    throw "Inferrance error: Cannot infer a named implicit lambda";
}
inferrance_t ru_t::infer(context_t&){
    // LOG("Inferring U");
    // LOG("Inferrance of U done");
    return inferrance_t(std::make_shared<u_t>(), VU);
}
inferrance_t rvar_t::infer(context_t& cont){
    if (cont.types.contains(name)) {
        auto res = cont.types.at(name).end()-1;
        term_ptr term = std::make_shared<var_t>(cont.level-1-res->level);
        LOG("Variable  " << name << " inferred with type " << *res->typ);
        return inferrance_t(term,res->typ->clone());
    }
    else {
        throw "Inferrance error: Unbounded variable "+name;
    }
}
inferrance_t rapp_t::infer(context_t& cont) {
    LOG("Inferring Explicit app " << *this->left << " and " << *this->right);
    inferrance_t inff = left->infer(cont);
    inff = inff.typ->force()->insert(cont,inff.term);
    std::pair<value_ptr,closure_t> infr = inff.typ->force()->infer_RAPP(cont);
    term_ptr rterm = right->check(cont,infr.first);
    CAPP(rterm->eval(cont.environment),infr.second,typ)
    term_ptr res = std::make_shared<app_t>(inff.term,rterm);
    LOG("Explicit App " << *this << " inferred with type " << *typ);
    return inferrance_t(res,typ);
}
inferrance_t riapp_t::infer(context_t& cont) {
    LOG("Inferring Implicit app " << *this->left << " and " << *this->right);
    inferrance_t inff = left->infer(cont);
    std::pair<value_ptr,closure_t> infr = inff.typ->force()->infer_RINAPP(cont);
    term_ptr rterm = right->check(cont,infr.first);
    CAPP(rterm->eval(cont.environment),infr.second,typ)
    LOG("Argument of implicit app of type " << *infr.second.term);
    term_ptr res = std::make_shared<iapp_t>(inff.term,rterm);
    LOG("Implicit App " << *this << " inferred with type " << *typ);
    return inferrance_t(res,typ);
}
inferrance_t rnapp_t::infer(context_t& cont) {
    LOG("Inferring Named implicit app " << *this->left << " and " << *this->right);
    inferrance_t inff = left->infer(cont);
    inff = inff.typ->force()->insertUntilName(cont,ivar,inff.term);
    std::pair<value_ptr,closure_t> infr = inff.typ->force()->infer_RINAPP(cont);
    term_ptr rterm = right->check(cont,infr.first);
    CAPP(rterm->eval(cont.environment),infr.second,typ)
    term_ptr res = std::make_shared<iapp_t>(inff.term,rterm);
    LOG("Named Implicit App " << *this << " inferred with type " << *typ);
    return inferrance_t(res,typ);
}
inferrance_t rpi_t::infer(context_t& cont) {
    LOG("Inferring Explicit pi " << *this);
    term_ptr ttyp = typ->check(cont,VU);
    cont.new_var(var,ttyp->eval(cont.environment));
    term_ptr tbody = body->check(cont,VU);
    cont.pop(var);
    term_ptr res = std::make_shared<pi_t>(var,ttyp,tbody);
    LOG("Pi " << *this << " inferred with type U");
    return inferrance_t(res,VU);
}
inferrance_t ripi_t::infer(context_t& cont) {
    LOG("Inferring Implicit pi " << *this);
    term_ptr ttyp = typ->check(cont,VU);
    cont.new_var(var,ttyp->eval(cont.environment));
    term_ptr tbody = body->check(cont,VU);
    cont.pop(var);
    term_ptr res = std::make_shared<ipi_t>(var,ttyp,tbody);
    LOG("Implicit Pi " << *this << " inferred with type U");
    return inferrance_t(res,VU);
}
inferrance_t rlet_t::infer(context_t& cont) {
    LOG("Inferring Let " << var << " : " << *typ << " = " << *def << " in [...]");
    term_ptr ttyp = typ->check(cont,VU);
    LOG("Let's see the type before eval: " << *ttyp);
    value_ptr vtyp = ttyp->eval(cont.environment);
    term_ptr tdef = def->check(cont,vtyp);
    value_ptr vdef = tdef->eval(cont.environment);
    cont.new_val(var,ttyp,vtyp,tdef,vdef);
    inferrance_t inf = body->infer(cont);
    LOG("Let " << var << " : " << *typ << " = " << *def << " in [...] inferred with type " << *inf.typ);
    cont.pop(var);
    inferrance_t res = inferrance_t(std::make_shared<let_t>(var,ttyp,tdef,inf.term),inf.typ);
    return res;
}
inferrance_t rhole_t::infer(context_t& cont) {
    LOG("Inferring hole");
    value_ptr a = FRESHMETA(VU)->eval(cont.environment);
    LOG("Inferrance of hole successfule, inferred with type " << *a);
    return inferrance_t(FRESHMETA(a),a);
}


raw_ptr raw_t::build(raw_ptr) {throw "Parser error: Cannot build a non-utility raw type";}
raw_ptr iicit::build(raw_ptr r) {return std::make_shared<riabs_t>(bind,r);}
raw_ptr eicit::build(raw_ptr r) {return std::make_shared<rabs_t>(bind,r);}
raw_ptr tiicit::build(raw_ptr r) {return std::make_shared<rtiabs_t>(bind,typ,r);}
raw_ptr teicit::build(raw_ptr r) {return std::make_shared<rtabs_t>(bind,typ,r);}
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
raw_ptr raw_t::auto_build() {throw "Parser error: Cannot auto build a non-list of arguments";}
raw_ptr arglist_t::auto_build() {
    raw_ptr res = args[0];
    for (auto it = args.begin()+1; it!=args.end(); it++) {
        res = (*it)->build(res);
    }
    return res;
}

void raw_t::push_back(raw_ptr) {throw "Parser error: Cannot push_back on a non-list type";}
void pibinderlist_t::push_back(raw_ptr r) {binders.push_back(r);}
void arglist_t::push_back(raw_ptr r) {args.push_back(r);}
void icitlist_t::push_back(raw_ptr r) {icits.push_back(r);}

std::vector<name_t>* raw_t::get_namelist() { throw "Parser error: Not a name list"; }
std::vector<name_t>* namelist_t::get_namelist() { return names; }