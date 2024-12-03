#include "value.hpp"

#define CAPP(v,body,val) \
    body.environment.push_back(v); \
    value_ptr val = body.term->eval(body.environment); \
    body.environment.pop_back(); 
#define TCAPP(v) CAPP(v,this->body,val)
#define VARL std::make_shared<vvar_t>(l)
#define VCAPP(body,val) CAPP(VARL,body,val)
#define VTCAPP TCAPP(VARL)
#define CONVVAPP(v) vapp_t(v,VARL).conv(l+1,val)

std::ostream& value_t::to_string(std::ostream& out) {return out << "Unknown value";}
std::ostream& vvar_t::to_string(std::ostream& out) {return out << "Var" << level;}
std::ostream& vabs_t::to_string(std::ostream& out) {return out << "λ " << body;}
std::ostream& vapp_t::to_string(std::ostream& out) {return out << "(" << left << " " << right << ")";}
std::ostream& vu_t::to_string(std::ostream& out) {return out << "𝒰";}
std::ostream& vpi_t::to_string(std::ostream& out) {return out << "(" << var << " : " << typ << ") → " << body;}

std::ostream& operator<< (std::ostream& out, const environment_t& env) {
    int com = 0;
    out << "[";
    for (auto it = env.begin(); it != env.end(); it++) {
        out << ((com==0)? "": ", ") << com << " => " << **it;
    }
    return out << "]";
}

std::ostream& operator<< (std::ostream& out, const closure_t& clo) {
    return out << clo.environment << " " << clo.term;
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

term_ptr value_t::quote(int) {
    throw "Reification of an unknown value.";
}
term_ptr vvar_t::quote(int l) {
    return std::make_shared<var_t>(l - level -1);
}
term_ptr vabs_t::quote(int l) {
    VTCAPP;
    return std::make_shared<abs_t>(var,val->quote(l+1));
}
term_ptr vapp_t::quote(int l) {
    return std::make_shared<app_t>(left->quote(l),right->quote(l));
}
term_ptr vu_t::quote(int) {
    return std::make_shared<u_t>();
}
term_ptr vpi_t::quote(int l) {
    VTCAPP;
    return std::make_shared<pi_t>(var,typ->quote(l),val->quote(l+1));
}

bool value_t::conv_VU(int) {return false;}
bool vu_t::conv_VU(int) {return true;}
bool vabs_t::conv_VU(int l) {
    VTCAPP;
    return CONVVAPP(std::make_shared<vu_t>());
}

bool value_t::conv_VPI(int, value_ptr, closure_t&) {return false;}
bool vpi_t::conv_VPI(int l, value_ptr typ2, closure_t& clo) {
    VTCAPP;
    VCAPP(clo,val2);
    return typ2->conv(l,typ) && val2->conv(l+1,val);
}
bool vabs_t::conv_VPI(int l, value_ptr typ2, closure_t& clo) {
    VTCAPP;
    return CONVVAPP(std::make_shared<vpi_t>(" ",typ2,clo));
}

bool value_t::conv_VABS(int l, closure_t& clo) {
    VCAPP(clo,val);
    return CONVVAPP(shared_from_this());
}
bool vabs_t::conv_VABS(int l, closure_t& clo) {
    VTCAPP;
    VCAPP(clo,val2);
    return val2->conv(l+1,val);
}

bool value_t::conv_VAPP(int, value_ptr, value_ptr) {return false;}
bool vapp_t::conv_VAPP(int l, value_ptr a, value_ptr b) {
    return a->conv(l,left) && b->conv(l,right);
}
bool vabs_t::conv_VAPP(int l, value_ptr a, value_ptr b) {
    VTCAPP;
    return CONVVAPP(std::make_shared<vapp_t>(a,b));
}

bool value_t::conv_VVAR(int, int) {return false;}
bool vvar_t::conv_VVAR(int, int l) {
    return level == l;
}
bool vabs_t::conv_VVAR(int l, int level) {
    VTCAPP;
    return CONVVAPP(std::make_shared<vvar_t>(level));
}

bool value_t::conv(int, value_ptr) {return false;}
bool vu_t::conv(int l, value_ptr v) {return v->conv_VU(l);}
bool vpi_t::conv(int l, value_ptr v) {
    return v->conv_VPI(l,typ,body);
}
bool vabs_t::conv(int l, value_ptr v) {
    return v->conv_VABS(l,body);
}
bool vapp_t::conv(int l, value_ptr v) {
    return v->conv_VAPP(l,left,right);
} 
bool vvar_t::conv(int l, value_ptr v) {
    return v->conv_VVAR(l,level);
} 