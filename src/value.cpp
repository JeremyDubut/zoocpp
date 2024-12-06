#include <sstream>
#include "value.hpp"
#include "rsyntax.hpp"
#include "syntax.hpp"

#define CAPP(v,body,val) \
    body.environment.push_back(v); \
    value_ptr val = body.term->eval(body.environment); \
    body.environment.pop_back(); 
#define TCAPP(v) CAPP(v,this->body,val)
#define VARL std::make_shared<vvar_t>(l)
#define VCAPP(body,val) CAPP(VARL,body,val)
#define VTCAPP TCAPP(VARL)
#define CONVVAPP(v) vapp_t(v,VARL).conv(l+1,val)
#define INFERFUN \
    std::stringstream ss(""); \
    ss << "Expected a function type and received type: " << *this; \
    throw ss.str();


std::ostream& value_t::to_string(std::ostream& out) {return out << "Unknown value";}
std::ostream& vvar_t::to_string(std::ostream& out) {return out << "Var" << level;}
std::ostream& vabs_t::to_string(std::ostream& out) {return out << "λ " << body;}
std::ostream& vapp_t::to_string(std::ostream& out) {return out << "(" << *left << " " << *right << ")";}
std::ostream& vu_t::to_string(std::ostream& out) {return out << "𝒰";}
std::ostream& vpi_t::to_string(std::ostream& out) {return out << "(" << var << " : " << *typ << ") → " << body;}

std::ostream& operator<< (std::ostream& out, const environment_t& env) {
    std::size_t com = 0;
    out << "[";
    for (auto it = env.begin(); it != env.end(); it++) {
        out << ((com==0)? "": ", ") << com << " => " << **it;
        com++;
    }
    return out << "]";
}

std::ostream& operator<< (std::ostream& out, const closure_t& clo) {
    return out << clo.environment << " " << *clo.term;
}

std::ostream& operator<< (std::ostream& out, value_t& value) {
    return value.to_string(out);
}

value_ptr value_t::eval_in_abs(value_ptr v) {
    return std::make_shared<vapp_t>(this->shared_from_this(),v);
}
value_ptr vabs_t::eval_in_abs(value_ptr v) {
    TCAPP(v);
    return val;
}

term_ptr value_t::quote(std::size_t) {
    throw "Reification of an unknown value.";
}
term_ptr vvar_t::quote(std::size_t l) {
    return std::make_shared<var_t>(l - level -1);
}
term_ptr vabs_t::quote(std::size_t l) {
    VTCAPP;
    return std::make_shared<abs_t>(var,val->quote(l+1));
}
term_ptr vapp_t::quote(std::size_t l) {
    return std::make_shared<app_t>(left->quote(l),right->quote(l));
}
term_ptr vu_t::quote(std::size_t) {
    return std::make_shared<u_t>();
}
term_ptr vpi_t::quote(std::size_t l) {
    VTCAPP;
    return std::make_shared<pi_t>(var,typ->quote(l),val->quote(l+1));
}

bool value_t::conv_VU(std::size_t) {return false;}
bool vu_t::conv_VU(std::size_t) {return true;}
bool vabs_t::conv_VU(std::size_t l) {
    VTCAPP;
    return CONVVAPP(std::make_shared<vu_t>());
}

bool value_t::conv_VPI(std::size_t, value_ptr, closure_t&) {return false;}
bool vpi_t::conv_VPI(std::size_t l, value_ptr typ2, closure_t& clo) {
    VTCAPP;
    VCAPP(clo,val2);
    return typ2->conv(l,typ) && val2->conv(l+1,val);
}
bool vabs_t::conv_VPI(std::size_t l, value_ptr typ2, closure_t& clo) {
    VTCAPP;
    return CONVVAPP(std::make_shared<vpi_t>(" ",typ2,clo));
}

bool value_t::conv_VABS(std::size_t l, closure_t& clo) {
    VCAPP(clo,val);
    return CONVVAPP(shared_from_this());
}
bool vabs_t::conv_VABS(std::size_t l, closure_t& clo) {
    VTCAPP;
    VCAPP(clo,val2);
    return val2->conv(l+1,val);
}

bool value_t::conv_VAPP(std::size_t, value_ptr, value_ptr) {return false;}
bool vapp_t::conv_VAPP(std::size_t l, value_ptr a, value_ptr b) {
    return a->conv(l,left) && b->conv(l,right);
}
bool vabs_t::conv_VAPP(std::size_t l, value_ptr a, value_ptr b) {
    VTCAPP;
    return CONVVAPP(std::make_shared<vapp_t>(a,b));
}

bool value_t::conv_VVAR(std::size_t, std::size_t) {return false;}
bool vvar_t::conv_VVAR(std::size_t, std::size_t l) {
    return level == l;
}
bool vabs_t::conv_VVAR(std::size_t l, std::size_t level) {
    VTCAPP;
    return CONVVAPP(std::make_shared<vvar_t>(level));
}

bool value_t::conv(std::size_t, value_ptr) {return false;}
bool vu_t::conv(std::size_t l, value_ptr v) {return v->conv_VU(l);}
bool vpi_t::conv(std::size_t l, value_ptr v) {
    return v->conv_VPI(l,typ,body);
}
bool vabs_t::conv(std::size_t l, value_ptr v) {
    return v->conv_VABS(l,body);
}
bool vapp_t::conv(std::size_t l, value_ptr v) {
    return v->conv_VAPP(l,left,right);
} 
bool vvar_t::conv(std::size_t l, value_ptr v) {
    return v->conv_VVAR(l,level);
} 

term_ptr value_t::check_RABS(context_t&,name_t, raw_ptr) {
    INFERFUN;
}
term_ptr vpi_t::check_RABS(context_t& cont,name_t var, raw_ptr r) {
    // std::cout << "Checking inside lam " << var << " and " << *r << " of type " << *this << std::endl;
    // std::cout << cont;
    cont.new_var(var,typ);
    // std::cout << cont;
    TCAPP(std::make_shared<vvar_t>(cont.level-1));
    term_ptr res = std::make_shared<abs_t>(var, r->check(cont,val));
    cont.pop(var);
    return res;
}

inferrance_t value_t::infer_RAPP(context_t&,term_ptr,raw_ptr) {
    INFERFUN;
}
inferrance_t vpi_t::infer_RAPP(context_t& cont, term_ptr left, raw_ptr right) {
    // std::cout << "Inferring function type for " << *left << " with type " << *this << std::endl;
    // std::cout << " in env " << body.environment << std::endl;
    term_ptr rterm = right->check(cont, typ);
    // std::cout << "Evaluating right term of app " << *rterm << " in env " << body.environment << std::endl;
    value_ptr rvalue = rterm->eval(cont.environment);
    // std::cout << "Right term of app " << *rterm << " evaluated as " << *rvalue << std::endl;
    TCAPP(rvalue);
    return inferrance_t(std::make_shared<app_t>(left,rterm),val);
}


