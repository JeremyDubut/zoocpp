#include "value.hpp"
#include "rsyntax.hpp"
#include "syntax.hpp"
#include "metavar.hpp"
#include "errors.hpp"

#define TCAPP(v) CAPP(v,this->body,val)
#define VARL std::make_shared<vrig_t>(l)
#define VCAPP(body,val) CAPP(VARL,body,val)
#define VTCAPP TCAPP(VARL)
#define FUNSP(v,fun,spine) \
    term_ptr trhs = v; \
    for (std::pair<value_ptr,bool> it : spine) { \
        term_ptr term = it.first->clone()->fun; \
        if (it.second) { \
            trhs = std::make_shared<iapp_t>(trhs,term); \
        } \
        else { \
            trhs = std::make_shared<app_t>(trhs,term); \
        } \
    } 
#define QUOTESP(v) \
    FUNSP(v,quote(l),spine)\
    return trhs; 
#define RENAMESP_NORET(v,m,spine) FUNSP(v,force()->rename(m,ren),spine)
#define RENAMESP(v,m,spine) \
    RENAMESP_NORET(v,m,spine) \
    return trhs;
#define UNIFYSP \
    auto it1 = spine1.begin(); \
    for (auto it : spine) { \
        LOG("Unifying spine: " << *(*it1).first << " and " << *it.first); \
        (*it1).first->clone()->force()->unify(l,it.first->clone()); \
        it1++; \
    }
#define RENAMEABS(t) \
    std::size_t l = ren.cod; \
    VTCAPP; \
    ren.lift(); \
    term_ptr res = val->force()->rename(m,ren); \
    ren.pop(); \
    return std::make_shared<t>(var,res);
#define RENAMEPI(t) \
    std::size_t l = ren.cod; \
    VTCAPP \
    ren.lift(); \
    term_ptr res = val->force()->rename(m,ren); \
    ren.pop(); \
    return std::make_shared<t>(var,typ->force()->rename(m,ren),res);


std::ostream& value_t::to_string(std::ostream& out) {return out << "Unknown value";}
std::ostream& vabs_t::to_string(std::ostream& out) {return out << "λ " << body;}
std::ostream& viabs_t::to_string(std::ostream& out) {return out << "λ{} " << body;}
std::ostream& vu_t::to_string(std::ostream& out) {return out << "𝒰";}
std::ostream& vpi_t::to_string(std::ostream& out) {return out << "(" << var << " : " << *typ << ") → " << body;}
std::ostream& vipi_t::to_string(std::ostream& out) {return out << "{" << var << " : " << *typ << "} → " << body;}
std::ostream& vflex_t::to_string(std::ostream& out) {
    out << "??" << index << " [";
    for (auto it : spine) {
        out << *it.first->force() <<" ;";
    }
    return out << "]";
}
std::ostream& vrig_t::to_string(std::ostream& out) {
    out << "?" << level << " [";
    for (auto it : spine) {
        out << *it.first <<" ;";
    }
    return out << "]";
}

// Environments take too long to print
// removed them for now
std::ostream& operator<< (std::ostream& out, const environment_t& env) {
    // std::size_t com = 0;
    out << "[env " << env.size();
    // for (auto it = env.begin(); it != env.end(); it++) {
    //     out << ((com==0)? "": ", ") << com << " => " << **it;
    //     com++;
    // }
    return out << "]";
}
std::ostream& operator<< (std::ostream& out, const closure_t& clo) {
    return out << clo.environment << " " << *clo.term;
}
std::ostream& operator<< (std::ostream& out, value_t& value) {
    return value.to_string(out);
}

value_ptr value_t::vApp(value_ptr, bool) {
    throw beta_red_e(shared_from_this());
}
value_ptr vabs_t::vApp(value_ptr v, bool) {
    TCAPP(v);
    return val;
}
value_ptr vflex_t::vApp(value_ptr v, bool icit) {
    // spine.push_back(std::make_pair(v,icit));
    // return shared_from_this();
    spine_t spine_cp = spine;
    spine_cp.push_back(std::make_pair(v,icit));
    return std::make_shared<vflex_t>(index,spine_cp);
}
value_ptr vrig_t::vApp(value_ptr v, bool icit) {
    // spine.push_back(std::make_pair(v,icit));
    // return shared_from_this();
    spine_t spine_cp = spine;
    spine_cp.push_back(std::make_pair(v,icit));
    return std::make_shared<vrig_t>(level,spine_cp);
}

value_ptr value_t::vAppSp(spine_t& spine) {
    value_ptr res = shared_from_this();
    for (std::pair<value_ptr,bool> it : spine) {
        res = res->vApp(it.first->clone(),it.second);
    }
    return res;
}

term_ptr value_t::quote(std::size_t) {
    throw reification_e(shared_from_this());
}
term_ptr vabs_t::quote(std::size_t l) {
    VTCAPP;
    return std::make_shared<abs_t>(var,val->quote(l+1));
}
term_ptr viabs_t::quote(std::size_t l) {
    VTCAPP;
    return std::make_shared<iabs_t>(var,val->quote(l+1));
}
term_ptr vu_t::quote(std::size_t) {
    return std::make_shared<u_t>();
}
term_ptr vpi_t::quote(std::size_t l) {
    VTCAPP;
    return std::make_shared<pi_t>(var,typ->quote(l),val->quote(l+1));
}
term_ptr vipi_t::quote(std::size_t l) {
    VTCAPP;
    return std::make_shared<ipi_t>(var,typ->quote(l),val->quote(l+1));
}
term_ptr vflex_t::quote(std::size_t l) {
    QUOTESP(std::make_shared<meta_t>(index))
}
term_ptr vrig_t::quote(std::size_t l) {
    QUOTESP(std::make_shared<var_t>(l-level-1))
}

term_ptr value_t::check_VAR(context_t& cont, name_t var) {
    if (cont.types.contains(var)) {
        auto res = cont.types.at(var).end()-1;
        term_ptr term = std::make_shared<var_t>(cont.level-1-res->level);
        inferrance_t inf = res->typ->clone()->force()->insert(cont,term);
        unify(cont.level,inf.typ);
        return inf.term;
    }
    else {
        throw unbound_var_e(var);
    }
}
term_ptr vipi_t::check_VAR(context_t& cont,name_t name) {
    LOG("Checking VAR with VIPI");
    if (cont.types.contains(name)) {
        auto res = cont.types.at(name).end()-1;
        return res->typ->clone()->force()->check_VAR_VIPI(cont,name,var,typ,body,res->level);
    }
    else {
        TCAPP(std::make_shared<vrig_t>(cont.level));
        cont.new_bind(this->var,this->typ);
        term_ptr res = std::make_shared<iabs_t>(this->var, val->force()->check_VAR(cont,name));
        cont.pop();
        return res;
    }
}
term_ptr vflex_t::check_VAR(context_t& cont,name_t var) {
    FUNSP(std::make_shared<meta_t>(index),quote(cont.level),spine)
    std::size_t placeholder = metavar_t(cont.local->closety(trhs)->eval()).id;
    std::size_t c = check_t(cont,std::make_shared<rvar_t>(var),shared_from_this(),placeholder).id;
    metavar_t::lookup(index)->add_block(c);
    return std::make_shared<tcheck_t>(c);
}
term_ptr value_t::check_VAR_VIPI(context_t& cont,name_t name,name_t var,value_ptr typ,closure_t& body, std::size_t){
    std::size_t l = cont.level;
    VCAPP(body,val);
    cont.new_bind(var,typ);
    term_ptr res = val->force()->check_VAR(cont,name);
    cont.pop();
    return std::make_shared<iabs_t>(var,res);
};
term_ptr vflex_t::check_VAR_VIPI(context_t& cont,name_t,name_t var,value_ptr typ,closure_t& body,std::size_t level){
    LOG("Checking VAR with VIPI and VFLEX");
    renaming_t ren = renaming_t(cont.level,spine);
    auto data = metavar_t::lookup(index)->read_unsolved();
    if (ren.prune.has_value()) {
        data.second->pruneTy(ren.prune.value());
    }
    std::size_t l = ren.cod; 
    VCAPP(body,val)
    ren.lift(); 
    term_ptr res = val->force()->rename(index,ren); 
    ren.pop(); 
    term_ptr term = std::make_shared<ipi_t>(var,typ->force()->rename(index,ren),res);
    value_ptr sol = data.second->force()->wrapAbs(ren.dom,term)->eval();
    metavar_t::lookupTable[index] = metavar_t::lookup(index)->update(sol);
    for (std::size_t it : data.first) {
        check_t::lookup(it)->retry(it);
    }
    return std::make_shared<var_t>(cont.level-level-1);
};
term_ptr value_t::check_RABS(context_t& cont,name_t var, raw_ptr body) {
    value_ptr a = FRESHMETA(VU)->eval(cont.environment); // Type of FM
    cont.new_var(var,a);
    inferrance_t inf = body->infer(cont);
    cont.pop(var);
    closure_t clos = closure_t(cont.environment,inf.typ->quote(cont.level+1));
    unify(cont.level,std::make_shared<vpi_t>(var,a,clos));
    return std::make_shared<abs_t>(var,inf.term);
}
term_ptr vpi_t::check_RABS(context_t& cont,name_t var, raw_ptr r) {
    TCAPP(std::make_shared<vrig_t>(cont.level));
    cont.new_var(var,typ);
    term_ptr res = std::make_shared<abs_t>(var, r->check(cont,val));
    cont.pop(var);
    return res;
}
term_ptr vipi_t::check_RABS(context_t& cont,name_t var, raw_ptr r) {
    TCAPP(std::make_shared<vrig_t>(cont.level));
    cont.new_bind(this->var,this->typ);
    term_ptr res = std::make_shared<iabs_t>(this->var, val->force()->check_RABS(cont,var,r));
    cont.pop();
    return res;
}
term_ptr vflex_t::check_RABS(context_t& cont,name_t var, raw_ptr r) {
    FUNSP(std::make_shared<meta_t>(index),quote(cont.level),spine)
    std::size_t placeholder = metavar_t(cont.local->closety(trhs)->eval()).id;
    std::size_t c = check_t(cont,std::make_shared<rabs_t>(var,r),shared_from_this(),placeholder).id;
    metavar_t::lookup(index)->add_block(c);
    return std::make_shared<tcheck_t>(c);
}
term_ptr value_t::check_RTABS(context_t& cont,name_t var, raw_ptr typ, raw_ptr body) {
    value_ptr a = typ->check(cont,VU)->eval(cont.environment); 
    cont.new_var(var,a);
    inferrance_t inf = body->infer(cont);
    cont.pop(var);
    closure_t clos = closure_t(cont.environment,inf.typ->quote(cont.level+1));
    unify(cont.level,std::make_shared<vpi_t>(var,a,clos));
    return std::make_shared<abs_t>(var,inf.term);
}
term_ptr vpi_t::check_RTABS(context_t& cont,name_t var, raw_ptr typl, raw_ptr r) {
    term_ptr ttypl = typl->check(cont,VU);
    ttypl->eval(cont.environment)->force()->unify(cont.level,typ);
    TCAPP(std::make_shared<vrig_t>(cont.level));
    cont.new_var(var,typ);
    term_ptr res = std::make_shared<abs_t>(var, r->check(cont,val));
    cont.pop(var);
    return res;
}
term_ptr vipi_t::check_RTABS(context_t& cont,name_t var, raw_ptr typ, raw_ptr r) {
    TCAPP(std::make_shared<vrig_t>(cont.level));
    cont.new_bind(this->var,this->typ);
    term_ptr res = std::make_shared<iabs_t>(this->var, val->force()->check_RTABS(cont,var,typ,r));
    cont.pop();
    return res;
}
term_ptr vflex_t::check_RTABS(context_t& cont,name_t var, raw_ptr typ, raw_ptr r) {
    FUNSP(std::make_shared<meta_t>(index),quote(cont.level),spine)
    std::size_t placeholder = metavar_t(cont.local->closety(trhs)->eval()).id;
    std::size_t c = check_t(cont,std::make_shared<rtabs_t>(var,typ,r),shared_from_this(),placeholder).id;
    metavar_t::lookup(index)->add_block(c);
    return std::make_shared<tcheck_t>(c);
}
term_ptr value_t::check_RIABS(context_t& cont,name_t var, raw_ptr body) {
    value_ptr a = FRESHMETA(VU)->eval(cont.environment); // Type of FM
    cont.new_var(var,a);
    inferrance_t inf = body->infer(cont);
    cont.pop(var);
    closure_t clos = closure_t(cont.environment,inf.typ->quote(cont.level+1));
    unify(cont.level,std::make_shared<vipi_t>(var,a,clos));
    return std::make_shared<iabs_t>(var,inf.term);
}
term_ptr vipi_t::check_RIABS(context_t& cont,name_t var, raw_ptr r) {
    TCAPP(std::make_shared<vrig_t>(cont.level));
    cont.new_var(var,typ);
    term_ptr res = std::make_shared<iabs_t>(var, r->check(cont,val));
    cont.pop(var);
    return res;
}
term_ptr vflex_t::check_RIABS(context_t& cont,name_t var, raw_ptr r) {
    FUNSP(std::make_shared<meta_t>(index),quote(cont.level),spine)
    std::size_t placeholder = metavar_t(cont.local->closety(trhs)->eval()).id;
    std::size_t c = check_t(cont,std::make_shared<riabs_t>(var,r),shared_from_this(),placeholder).id;
    metavar_t::lookup(index)->add_block(c);
    return std::make_shared<tcheck_t>(c);
}
term_ptr value_t::check_RTIABS(context_t& cont,name_t var, raw_ptr typ, raw_ptr body) {
    value_ptr a = typ->check(cont,VU)->eval(cont.environment); 
    cont.new_var(var,a);
    inferrance_t inf = body->infer(cont);
    cont.pop(var);
    closure_t clos = closure_t(cont.environment,inf.typ->quote(cont.level+1));
    unify(cont.level,std::make_shared<vipi_t>(var,a,clos));
    return std::make_shared<iabs_t>(var,inf.term);
}
term_ptr vipi_t::check_RTIABS(context_t& cont,name_t var, raw_ptr typl, raw_ptr r) {
    term_ptr ttypl = typl->check(cont,VU);
    ttypl->eval(cont.environment)->force()->unify(cont.level,typ);
    TCAPP(std::make_shared<vrig_t>(cont.level));
    cont.new_var(var,typ);
    term_ptr res = std::make_shared<iabs_t>(var, r->check(cont,val));
    cont.pop(var);
    return res;
}
term_ptr vflex_t::check_RTIABS(context_t& cont,name_t var, raw_ptr typl, raw_ptr r) {
    FUNSP(std::make_shared<meta_t>(index),quote(cont.level),spine)
    std::size_t placeholder = metavar_t(cont.local->closety(trhs)->eval()).id;
    std::size_t c = check_t(cont,std::make_shared<rtiabs_t>(var,typl,r),shared_from_this(),placeholder).id;
    metavar_t::lookup(index)->add_block(c);
    return std::make_shared<tcheck_t>(c);
}
term_ptr value_t::check_RNABS(context_t& cont,name_t var, name_t, raw_ptr body) {
    value_ptr a = FRESHMETA(VU)->eval(cont.environment); // Type of FM
    cont.new_var(var,a);
    inferrance_t inf = body->infer(cont);
    cont.pop(var);
    closure_t clos = closure_t(cont.environment,inf.typ->quote(cont.level+1));
    unify(cont.level,std::make_shared<vipi_t>(var,a,clos));
    return std::make_shared<iabs_t>(var,inf.term);
}
term_ptr vipi_t::check_RNABS(context_t& cont,name_t var, name_t ivar, raw_ptr r) {
    if (ivar == this->var) {
        TCAPP(std::make_shared<vrig_t>(cont.level));
        cont.new_var(var,typ);
        term_ptr res = std::make_shared<iabs_t>(var, r->check(cont,val));
        cont.pop(var);
        return res;
    }
    else {
        TCAPP(std::make_shared<vrig_t>(cont.level));
        cont.new_bind(this->var,this->typ);
        term_ptr res = std::make_shared<iabs_t>(this->var, val->force()->check_RNABS(cont,var,ivar,r));
        cont.pop();
        return res;
    }
}
term_ptr vflex_t::check_RNABS(context_t& cont,name_t var, name_t ivar, raw_ptr r) {
    FUNSP(std::make_shared<meta_t>(index),quote(cont.level),spine)
    std::size_t placeholder = metavar_t(cont.local->closety(trhs)->eval()).id;
    std::size_t c = check_t(cont,std::make_shared<rnabs_t>(var,r,ivar),shared_from_this(),placeholder).id;
    metavar_t::lookup(index)->add_block(c);
    return std::make_shared<tcheck_t>(c);
}

term_ptr value_t::check_LET(context_t& cont,name_t var,raw_ptr typ,raw_ptr def,raw_ptr body) {
    term_ptr ta = typ->check(cont,VU);
    value_ptr va = ta->eval(cont.environment);
    term_ptr tt = def->check(cont,va);
    value_ptr vt = tt->eval(cont.environment);
    cont.new_val(var,ta,va,tt,vt);
    term_ptr tu = body->check(cont,shared_from_this());
    cont.pop(var);
    term_ptr res = std::make_shared<let_t>(var,ta,tt,tu);
    return res;
}
term_ptr vipi_t::check_LET(context_t& cont,name_t var,raw_ptr typ,raw_ptr def,raw_ptr body) {
    TCAPP(std::make_shared<vrig_t>(cont.level));
    cont.new_bind(this->var,this->typ);
    term_ptr res = std::make_shared<iabs_t>(this->var, val->force()->check_LET(cont,var,typ,def,body));
    cont.pop();
    return res;
}
term_ptr vflex_t::check_LET(context_t& cont, name_t var, raw_ptr typ, raw_ptr def, raw_ptr body) {
    FUNSP(std::make_shared<meta_t>(index),quote(cont.level),spine)
    std::size_t placeholder = metavar_t(cont.local->closety(trhs)->eval()).id;
    std::size_t c = check_t(cont,std::make_shared<rlet_t>(var,typ,def,body),shared_from_this(),placeholder).id;
    metavar_t::lookup(index)->add_block(c);
    return std::make_shared<tcheck_t>(c);
}

term_ptr value_t::check_HOLE(context_t& cont) {
    term_ptr res = FRESHMETA(shared_from_this()); // Type of FM
    return res;
}
term_ptr vipi_t::check_HOLE(context_t& cont) {
    TCAPP(std::make_shared<vrig_t>(cont.level));
    cont.new_bind(this->var,this->typ);
    term_ptr res = std::make_shared<iabs_t>(this->var, val->force()->check_HOLE(cont));
    cont.pop();
    return res;
}
term_ptr vflex_t::check_HOLE(context_t& cont) {
    FUNSP(std::make_shared<meta_t>(index),quote(cont.level),spine)
    std::size_t placeholder = metavar_t(cont.local->closety(trhs)->eval()).id;
    std::size_t c = check_t(cont,std::make_shared<rhole_t>(),shared_from_this(),placeholder).id;
    metavar_t::lookup(index)->add_block(c);
    return std::make_shared<tcheck_t>(c);
}

term_ptr value_t::check_RAW(context_t& cont,raw_ptr r) {
    inferrance_t inf = r->infer(cont);
    inf = inf.term->insert(cont,inf.typ);
    unify(cont.level,inf.typ);
    return inf.term;
}
term_ptr vipi_t::check_RAW(context_t& cont,raw_ptr r) {
    std::size_t l = cont.level;
    VTCAPP
    cont.new_bind(var,typ);
    term_ptr res = r->check(cont,val);
    cont.pop();
    return std::make_shared<iabs_t>(var,res);
}
term_ptr vflex_t::check_RAW(context_t& cont,raw_ptr r) {
    FUNSP(std::make_shared<meta_t>(index),quote(cont.level),spine)
    std::size_t placeholder = metavar_t(cont.local->closety(trhs)->eval()).id;
    std::size_t c = check_t(cont,r,shared_from_this(),placeholder).id;
    metavar_t::lookup(index)->add_block(c);
    return std::make_shared<tcheck_t>(c);
}

std::pair<value_ptr,closure_t> value_t::infer_RAPP(context_t& cont) {
    LOG("Infering an explicit app with value " << *this);
    value_ptr a = FRESHMETA(VU)->eval(cont.environment); // Type of FM
    std::stringstream ss("");
    ss << "x" << this;
    name_t var = ss.str();
    cont.new_var(var,a);
    term_ptr meta = FRESHMETA(VU);
    cont.pop(var);
    closure_t body = closure_t(cont.environment,meta); // Type of FM
    unify(cont.level,std::make_shared<vpi_t>(var,a,body));
    return std::make_pair(a,body);

}
std::pair<value_ptr,closure_t> vpi_t::infer_RAPP(context_t&) {
    LOG("Infering an explicit app with explicit pi " << *this);
    return std::make_pair(typ,body);
}
std::pair<value_ptr,closure_t> vipi_t::infer_RAPP(context_t&) {
    throw icit_mismatch_ei_e(shared_from_this());
}

std::pair<value_ptr,closure_t> value_t::infer_RINAPP(context_t& cont) {
    LOG("Inferrance inside implicit app, with " << *this);
    value_ptr a = FRESHMETA(VU)->eval(cont.environment); // Type of FM
    std::stringstream ss("");
    ss << "x" << this;
    name_t var = ss.str();
    cont.new_var(var,a);
    term_ptr meta = FRESHMETA(VU);
    cont.pop(var);
    closure_t body = closure_t(cont.environment,meta); // Type of FM
    unify(cont.level,std::make_shared<vipi_t>(var,a,body));
    return std::make_pair(a,body);

}
std::pair<value_ptr,closure_t> vipi_t::infer_RINAPP(context_t&) {
    LOG("Inferrance inside implicit app, with implicit pi");
    return std::make_pair(typ,body);
}
std::pair<value_ptr,closure_t> vpi_t::infer_RINAPP(context_t&) {
    throw icit_mismatch_ie_e(shared_from_this());
}
inferrance_t value_t::insertUntilName(context_t&,name_t ivar,term_ptr) {
    throw no_named_icit_arg_e(ivar,shared_from_this());
}
inferrance_t vipi_t::insertUntilName(context_t& cont,name_t ivar,term_ptr lterm) {
    if (ivar == var) {
        return inferrance_t(lterm,shared_from_this());
    }
    else {
        term_ptr m = FRESHMETA(typ); // Type of FM
        value_ptr mv = m->eval(cont.environment);
        TCAPP(mv)
        try {
            return val->force()->insertUntilName(cont,ivar,std::make_shared<iapp_t>(lterm,m));
        }
        catch (no_named_icit_arg_e& e) {
            e.val = shared_from_this();
            throw e;
        }
    }
}

value_ptr value_t::clone() {
    return shared_from_this();
}
value_ptr vpi_t::clone() {
    return std::make_shared<vpi_t>(var,typ->clone(),body);
}
value_ptr vipi_t::clone() {
    return std::make_shared<vipi_t>(var,typ->clone(),body);
}
value_ptr vflex_t::clone() {
    return std::make_shared<vflex_t>(index,spine);
}
value_ptr vrig_t::clone() {
    return std::make_shared<vrig_t>(level,spine);
}

value_ptr value_t::force() {
    return shared_from_this();
}
value_ptr vflex_t::force() {
    return metavar_t::lookup(index)->get_value(shared_from_this(),spine);
}

std::size_t value_t::inverse() {
    throw non_var_e(shared_from_this());
}
std::size_t vrig_t::inverse() {
    if (spine.empty()) {
        return level;
    }
    throw non_empty_spine_e(level,spine.size());
}

term_ptr value_t::wrapAbs(std::size_t level, term_ptr term) {
    try {
        return force()->wrapAbsRec(level,0,term);
    }
    catch (wrap_e& e) {
        e.typ = shared_from_this();
        throw e;
    }
}
term_ptr value_t::wrapAbsRec(std::size_t l, std::size_t lp, term_ptr term) {
    if (l == lp) {
        return term;
    }
    else {
        throw wrap_e(l);
    }
}
term_ptr vpi_t::wrapAbsRec(std::size_t l, std::size_t lp, term_ptr term) {
    if (l == lp) {
        return term;
    }
    else {
        TCAPP(std::make_shared<vrig_t>(lp));
        term_ptr lterm = val->force()->wrapAbsRec(l,lp+1,term);
        if (var == "_") {
            std::stringstream ss("");
            ss << "x" << lp;
            name_t name = ss.str();
            return std::make_shared<abs_t>(name,lterm);
        }
        else {
            return std::make_shared<abs_t>(var,lterm);
        }
    }
}
term_ptr vipi_t::wrapAbsRec(std::size_t l, std::size_t lp, term_ptr term) {
    if (l == lp) {
        return term;
    }
    else {
        TCAPP(std::make_shared<vrig_t>(lp));
        term_ptr lterm = val->force()->wrapAbsRec(l,lp+1,term);
        if (var == "_") {
            std::stringstream ss("");
            ss << "x" << lp;
            name_t name = ss.str();
            return std::make_shared<iabs_t>(name,lterm);
        }
        else {
            return std::make_shared<iabs_t>(var,lterm);
        }
    }
}

std::size_t pruneMeta(prunings_t& prune, std::size_t level) {
    LOG("Pruning metavariable " << level);
    std::pair<block_t,value_ptr> mtyp = metavar_t::lookup(level)->read_unsolved();
    value_ptr ptyp = mtyp.second->pruneTy(prune)->eval();
    std::size_t newl = metavar_t(ptyp,mtyp.first).id;
    term_ptr presol = std::make_shared<appp_t>(std::make_shared<meta_t>(newl),prune);
    value_ptr sol = mtyp.second->wrapAbs(prune.size(),presol)->eval();
    meta_ptr me = metavar_t::lookup(level)->update(sol); 
    metavar_t::lookupTable[level] = me;
    return newl;
}


term_ptr pruneVflex(std::optional<std::size_t> m, renaming_t& ren, std::size_t mp, spine_t& spine) {
    LOG("Pruning flex " << *std::make_shared<vflex_t>(mp,spine));
    tspine_t tspine {};
    status_t status = OK;
    for (auto it : spine) {
       it.first->force()->pruneVflexCases(m, ren, it.second, status, tspine);
    }
    std::size_t mpp;
    if (status == NeedsPruning) {
        LOG("Needs Pruning");
        prunings_t prune {};
        for (auto it : tspine) {
            if (!it.first.has_value()) {
                prune.push_back(None);
            }
            else if (it.second) {
                prune.push_back(Implicit);
            }
            else {
                prune.push_back(Explicit);
            }
        }
        mpp = pruneMeta(prune,mp);
    }
    else {
        LOG("Does Not Need Pruning");
        metavar_t::lookup(mp)->read_unsolved();
        mpp = mp;
    }
    term_ptr res = std::make_shared<meta_t>(mpp);
    for (auto it : tspine) {
        if (it.first.has_value()) {
            if (it.second) {
                res = std::make_shared<iapp_t>(res,it.first.value());
            }
            else {
                res = std::make_shared<app_t>(res,it.first.value());
            }
        }
    }
    return res;
}
void value_t::pruneVflexCases(
    std::optional<std::size_t> m,
    renaming_t& ren,
    bool icit,
    status_t& status,
    tspine_t& tspine) {

    switch (status) {
        case NeedsPruning: {
            throw prune_non_var_e(shared_from_this());
        }
        default: {
            tspine.push_back(std::make_pair(rename(m,ren),icit));
            status = OK_NonRenaming;
        }
    }
}
void vrig_t::pruneVflexCases(
    std::optional<std::size_t> m,
    renaming_t& ren,
    bool icit,
    status_t& status,
    tspine_t& tspine) {

    if (spine.empty()) {
        if (ren.ren.contains(level)) {
            tspine.push_back(std::make_pair(std::make_shared<var_t>(ren.dom-ren.ren[level]-1),icit));
        }
        else if (status == OK_NonRenaming) {
            throw spine_not_renaming_e(level);
        }
        else {
            tspine.push_back(std::make_pair(std::optional<term_ptr>(),icit));
            status = NeedsPruning;
        }
    }
    else {
        switch (status) {
            case NeedsPruning: {
                throw prune_non_empty_spine_e(level,spine.size());
            }
            default: {
                if (ren.ren.contains(level)) {
                    RENAMESP_NORET(std::make_shared<var_t>(ren.dom-ren.ren[level]-1),m.value(),spine)
                    tspine.push_back(std::make_pair(trhs,icit));
                    status = OK_NonRenaming;
                }
                else {
                    throw escape_rig_var_e(level);
                }
            }
        }
    }
}

term_ptr value_t::rename(std::optional<std::size_t>,renaming_t&) {
    throw rename_e(shared_from_this());
}
term_ptr vu_t::rename(std::optional<std::size_t>,renaming_t&) {
    return std::make_shared<u_t>();
}
term_ptr vabs_t::rename(std::optional<std::size_t> m,renaming_t& ren) {
    RENAMEABS(abs_t)
}
term_ptr viabs_t::rename(std::optional<std::size_t> m,renaming_t& ren) {
    RENAMEABS(iabs_t)
}
term_ptr vpi_t::rename(std::optional<std::size_t> m,renaming_t& ren) {
    RENAMEPI(pi_t)
}
term_ptr vipi_t::rename(std::optional<std::size_t> m,renaming_t& ren) {
    RENAMEPI(ipi_t)
}
term_ptr vflex_t::rename(std::optional<std::size_t> m,renaming_t& ren) {
    if (m.has_value() && index == m.value()) {
        throw occurs_check_e(index);
    }
    return pruneVflex(m,ren,index,spine);
}
term_ptr vrig_t::rename(std::optional<std::size_t> m,renaming_t& ren) {
    if (ren.ren.contains(level)) {
        RENAMESP(std::make_shared<var_t>(ren.dom-ren.ren[level]-1),m,spine)
    }
    else {
        throw rename_escape_rig_var_e(level);
    }
}

void value_t::solveWithRen(std::size_t m, renaming_t& ren) {
    meta_ptr meta = metavar_t::lookup(m);
    std::pair<block_t,value_ptr> mtyp = meta->read_unsolved();
    if (ren.prune.has_value()) {
        mtyp.second->pruneTy(ren.prune.value());
    }
    term_ptr term = force()->rename(m,ren);
    LOG("Renaming done");
    environment_t env {};
    value_ptr solution = mtyp.second->wrapAbs(ren.dom,term)->eval(env);
    metavar_t::lookupTable[m] = meta->update(solution);
    for (std::size_t it : mtyp.first) {
        check_t::lookup(it)->retry(it);
    }
}
void value_t::solve(std::size_t gamma, std::size_t index, spine_t& spine) {
    LOG("Solving " << *std::make_shared<vflex_t>(index,spine) << " with value " << *this << " solution? " << *metavar_t::lookup(index)->get_value(index));
    renaming_t ren = renaming_t(gamma,spine);
    solveWithRen(index,ren);
}

void value_t::unify(std::size_t,value_ptr) {
    throw unknown_unification_e(shared_from_this());
}
void vabs_t::unify(std::size_t l,value_ptr v) {
    LOG("Unifying Explicit Lam " << *this->quote(l) << " with " << *v->quote(l));  
    v->force()->unify_ABS(l,body);
    LOG("Unification of Explicit Lam " << *this->quote(l) << " with " << *v->quote(l) << " successful");
}
void viabs_t::unify(std::size_t l,value_ptr v) {
    LOG("Unifying Implicitl Lam " << *this->quote(l) << " with " << *v->quote(l)); 
    v->force()->unify_IABS(l,body);
    LOG("Unification of Implicit Lam " << *this->quote(l) << " with " << *v->quote(l) << " successful");
}
void vu_t::unify(std::size_t l,value_ptr v) {
    v->force()->unify_U(l);
}
void vpi_t::unify(std::size_t l,value_ptr v) {
    LOG("Unifying Explicit Pi " << *this->quote(l) << " with " << *v->quote(l)); 
    v->force()->unify_PI(l,var,typ,body);
    LOG("Unification of Explicit Pi " << *this->quote(l) << " with " << *v->quote(l) << " successful");
}
void vipi_t::unify(std::size_t l,value_ptr v) {
    LOG("Unifying Implicit Pi " << *this->quote(l) << " with " << *v->quote(l)); 
    v->force()->unify_IPI(l,var,typ,body);
    LOG("Unification of Implicit Pi " << *this->quote(l) << " with " << *v->quote(l) << " successful");
}
void vrig_t::unify(std::size_t l,value_ptr v) {
    LOG("Unifying RIG " << *this->quote(l) << " with " << *v->quote(l)); 
    v->force()->unify_RIG(l,level,spine);
    LOG("Unification of RIG " << *this->quote(l) << " with " << *v->quote(l) << " successful");
}
void vflex_t::unify(std::size_t l,value_ptr v) {
    LOG("Unifying FLEX " << *this << " with " << *v); 
    v->force()->unify_FLEX(l,index,spine);
    LOG("Unification of FLEX " << *this->quote(l) << " with " << *v << " successful");
}

void value_t::unify_ABS(std::size_t l ,closure_t& body) {
    VCAPP(body,val)
    val->force()->unify(l+1,vApp(VARL,false));
}
// Since viabs_t inherits from vabs_t, the same applies
void vabs_t::unify_ABS(std::size_t l ,closure_t& body1) {
    VCAPP(body1,val1)
    VTCAPP
    val1->force()->unify(l+1,val);
}
void value_t::unify_IABS(std::size_t l ,closure_t& body) {
    VCAPP(body,val)
    val->force()->unify(l+1,vApp(VARL,true));
}
void vabs_t::unify_IABS(std::size_t l ,closure_t& body1) {
    VCAPP(body1,val1)
    VTCAPP
    val1->force()->unify(l+1,val);
}
void value_t::unify_U(std::size_t) {
    throw rigid_u_e(shared_from_this());
}
void vu_t::unify_U(std::size_t) {}
// The case vabs_t is necessarily an error because vApp is not define on U
void vflex_t::unify_U(std::size_t l) {
    LOG("Solving " << *this << " with U");
    std::make_shared<vu_t>()->solve(l,index,spine);
}
void value_t::unify_PI(std::size_t,name_t,value_ptr,closure_t&) {
    throw rigid_pi_e(shared_from_this());
}
void vpi_t::unify_PI(std::size_t l,name_t,value_ptr typ1,closure_t& body1) {
    typ1->force()->unify(l,typ);
    VCAPP(body1,val1)
    VTCAPP
    val1->force()->unify(l+1,val);
}
void vipi_t::unify_PI(std::size_t,name_t,value_ptr,closure_t&) {
    throw icit_pi_e();
}
void vflex_t::unify_PI(std::size_t level, name_t var, value_ptr typ,closure_t& body) {
    LOG("Solving " << *this << " with explicit pi " << *typ << " and " << body);
    renaming_t ren = renaming_t(level,spine);
    meta_ptr meta = metavar_t::lookup(index);
    std::pair<block_t,value_ptr> mtyp = meta->read_unsolved();
    if (ren.prune.has_value()) {
        mtyp.second->pruneTy(ren.prune.value());
    }
    std::size_t l = ren.cod;
    VCAPP(body,val);
    ren.lift();
    term_ptr res = val->force()->rename(index,ren);
    ren.pop();
    term_ptr term = std::make_shared<pi_t>(var,typ->force()->rename(index,ren),res);
    environment_t env {};
    value_ptr solution = mtyp.second->wrapAbs(ren.dom,term)->eval(env);
    metavar_t::lookupTable[index] = meta->update(solution);
    for (std::size_t it : mtyp.first) {
        check_t::lookup(it)->retry(it);
    }
}
void value_t::unify_IPI(std::size_t,name_t,value_ptr,closure_t&) {
    throw rigid_ipi_e(shared_from_this());
}
void vipi_t::unify_IPI(std::size_t l,name_t,value_ptr typ1,closure_t& body1) {
    typ1->force()->unify(l,typ);
    VCAPP(body1,val1)
    VTCAPP
    val1->force()->unify(l+1,val);
}
void vflex_t::unify_IPI(std::size_t level, name_t var, value_ptr typ,closure_t& body) {
    LOG("Solving " << *this << " with implicit pi " << *typ << " and " << body);
    renaming_t ren = renaming_t(level,spine);
    meta_ptr meta = metavar_t::lookup(index);
    std::pair<block_t,value_ptr> mtyp = meta->read_unsolved();
    if (ren.prune.has_value()) {
        mtyp.second->pruneTy(ren.prune.value());
    }
    std::size_t l = ren.cod;
    VCAPP(body,val);
    ren.lift();
    term_ptr res = val->force()->rename(index,ren);
    ren.pop();
    term_ptr term = std::make_shared<ipi_t>(var,typ->force()->rename(index,ren),res);
    environment_t env {};
    value_ptr solution = mtyp.second->wrapAbs(ren.dom,term)->eval(env);
    metavar_t::lookupTable[index] = meta->update(solution);
    for (std::size_t it : mtyp.first) {
        check_t::lookup(it)->retry(it);
    }
}
void value_t::unify_RIG(std::size_t, std::size_t, spine_t&) {
    throw rigid_rig_e(shared_from_this());
}
void vrig_t::unify_RIG(std::size_t l, std::size_t m, spine_t& spine1) {
    if (m == level && spine.size() == spine1.size()) {
        UNIFYSP
    }
    else {
        throw rigid_rig_rig_e(m,level);
    }
}
void vabs_t::unify_RIG(std::size_t l, std::size_t m, spine_t& spine) {
    VTCAPP
    spine.push_back(std::make_pair(VARL,false));
    val->force()->unify_RIG(l+1,m,spine);
}
void viabs_t::unify_RIG(std::size_t l, std::size_t m, spine_t& spine) {
    VTCAPP
    spine.push_back(std::make_pair(VARL,true));
    val->force()->unify_RIG(l+1,m,spine);
}
void vflex_t::unify_RIG(std::size_t l, std::size_t level, spine_t& spine1) {
    renaming_t ren = renaming_t(l,spine);
    if (ren.ren.contains(level)) {
        LOG("Solving " << *this << " with RIG " << level);
        renaming_t ren = renaming_t(level,spine);
        meta_ptr meta = metavar_t::lookup(index);
        std::pair<block_t,value_ptr> mtyp = meta->read_unsolved();
        if (ren.prune.has_value()) {
            mtyp.second->pruneTy(ren.prune.value());
        }
        RENAMESP_NORET(std::make_shared<var_t>(ren.dom-ren.ren[level]-1),index,spine1)
        environment_t env {};
        value_ptr solution = mtyp.second->wrapAbs(ren.dom,trhs)->eval(env);
        metavar_t::lookupTable[index] = meta->update(solution);
        for (std::size_t it : mtyp.first) {
            check_t::lookup(it)->retry(it);
        }
    }
    else {
        throw escape_flex_var_e(level);
    }

}
void value_t::unify_FLEX(std::size_t l, std::size_t m, spine_t& spine) {
    solve(l,m,spine);
}
#define FLEXFLEX(m,sp,mp,spp) \
    renaming_t ren; \
    try { \
        LOG("Trying renaming"); \
        ren = renaming_t(l,sp); \
    } \
    catch (inverse_e& e) { \
        LOG("Catched renaming error " << e); \
        std::make_shared<vflex_t>(m,sp)->solve(l,mp,spp); \
        return; \
    } \
    std::make_shared<vflex_t>(mp,spp)->solveWithRen(m,ren); 
void vflex_t::unify_FLEX(std::size_t l, std::size_t m, spine_t& spine1) {
    if (m == index) {
        if (spine.size() != spine1.size()) {
            throw intersect_e(spine.size(),spine1.size());
        }
        bool pruneCheck = false;
        auto it1 = spine1.begin();
        prunings_t prune {};
        for (auto it : spine) {
            try {
                size_t x = it.first->force()->inverse();
                size_t x1 = it1->first->force()->inverse();
                if (x==x1) {
                    if (it1->second) {
                        prune.push_back(Implicit);
                    }
                    else {
                        prune.push_back(Explicit);
                    }
                }
                else {
                    prune.push_back(None);
                    pruneCheck = true;
                }
            }
            catch (inverse_e& e) {
                LOG("Catched inverve error");
                UNIFYSP
            }
        }
        if (pruneCheck) {
            pruneMeta(prune,m);
        }
    }
    else {
        if (spine.size() < spine1.size()) {
            FLEXFLEX(m,spine1,index,spine)
        }
        else {
            FLEXFLEX(index,spine,m,spine1)
        }
    }
}
void vabs_t::unify_FLEX(std::size_t l, std::size_t m, spine_t& spine) {
    VTCAPP
    spine.push_back(std::make_pair(VARL,false));
    val->force()->unify_FLEX(l+1,m,spine);
}
void viabs_t::unify_FLEX(std::size_t l, std::size_t m, spine_t& spine) {
    VTCAPP
    spine.push_back(std::make_pair(VARL,true));
    val->force()->unify_FLEX(l+1,m,spine);
}

inferrance_t value_t::insert(context_t&,term_ptr term) {
    return inferrance_t(term,shared_from_this());
}
inferrance_t vipi_t::insert(context_t& cont,term_ptr term) {
    LOG("Inserting an implicit");
    term_ptr m = FRESHMETA(typ); // Type of FM
    value_ptr a = m->eval(cont.environment);
    cont.new_var(var,a);
    TCAPP(a)
    cont.pop(var);
    return val->force()->insert(cont,std::make_shared<iapp_t>(term,m));
}

raw_ptr value_t::display() {return quote(0)->display();}

term_ptr value_t::pruneTy(prunings_t& prune) {
    LOG("Pruning " << *this << " with pruning " << prune.size());
    renaming_t ren = renaming_t(prune);
    return force()->pruneTyRec(0,ren);
}
term_ptr value_t::pruneTyRec(std::size_t i, renaming_t& ren) {
    LOG("Pruning arbitrary value " << i << " and " << ren.prune.value().size());
    if (ren.prune.value().size() == i) {
        return this->rename(std::optional<size_t>(),ren);
    }
    else {
        throw prune_non_pi_e(shared_from_this());
    }
}
term_ptr vpi_t::pruneTyRec(std::size_t i, renaming_t& ren) {
    LOG("Pruning explicit pi");
    if (ren.prune.value().size() == i) {
        return this->rename(std::optional<size_t>(),ren);
    }
    else {
        size_t l = ren.cod;
        VTCAPP;
        switch (ren.prune.value()[i]) {
            case (None): {
                LOG("None case");
                ren.skip();
                return val->force()->pruneTyRec(i+1,ren);
            }
            default: {
                LOG("Other case");
                term_ptr ttyp = typ->force()->rename(std::optional<std::size_t>(),ren);
                ren.lift();
                return std::make_shared<pi_t>(var,ttyp,val->force()->pruneTyRec(i+1,ren));
            }
        }
    }
}
term_ptr vipi_t::pruneTyRec(std::size_t i, renaming_t& ren) {
    LOG("Pruning implicit pi");
    if (ren.prune.value().size() == i) {
        return this->rename(std::optional<size_t>(),ren);
    }
    else {
        size_t l = ren.cod;
        VTCAPP;
        switch (ren.prune.value()[i]) {
            case (None): {
                ren.skip();
                return val->force()->pruneTyRec(i+1,ren);
            }
            default: {
                ren.lift();
                return std::make_shared<ipi_t>(var,typ->force()->rename(std::optional<std::size_t>(),ren),val->force()->pruneTyRec(i+1,ren));
            }
        }
    }
}

void value_t::retry(std::size_t c,context_t& cont,raw_ptr rterm,std::size_t m) {
    term_ptr term = rterm->check(cont,shared_from_this());
    metavar_t::lookup(m)->unify(m,cont,term);
    check_t::lookupTable[c] = std::make_shared<checked_t>(term);
}
void vflex_t::retry(std::size_t c,context_t&,raw_ptr,std::size_t) {
    metavar_t::lookup(index)->add_block(c);
}