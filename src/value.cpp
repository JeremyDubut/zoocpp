#include "value.hpp"

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

std::shared_ptr<value_t> value_t::eval_in_abs(std::shared_ptr<value_t> v) {
    return std::make_shared<vapp_t>(this->shared_from_this(),v);
}
std::shared_ptr<value_t> vabs_t::eval_in_abs(std::shared_ptr<value_t> v) {
    body.environment.push_back(v);
    std::shared_ptr<value_t> val = body.term->eval(body.environment);
    body.environment.pop_back();
    return val;
}

std::shared_ptr<term_t> value_t::quote(int) {
    throw "Reification of an unknown value.";
}
std::shared_ptr<term_t> vvar_t::quote(int l) {
    return std::make_shared<var_t>(l - level -1);
}
std::shared_ptr<term_t> vabs_t::quote(int l) {
    body.environment.push_back(std::make_shared<vvar_t>(l));
    std::shared_ptr<value_t> val = body.term->eval(body.environment);
    body.environment.pop_back();
    return std::make_shared<abs_t>(var,val->quote(l+1));
}
std::shared_ptr<term_t> vapp_t::quote(int l) {
    return std::make_shared<app_t>(left->quote(l),right->quote(l));
}
std::shared_ptr<term_t> vu_t::quote(int) {
    return std::make_shared<u_t>();
}
std::shared_ptr<term_t> vpi_t::quote(int l) {
    body.environment.push_back(std::make_shared<vvar_t>(l));
    std::shared_ptr<value_t> val = body.term->eval(body.environment);
    body.environment.pop_back();
    return std::make_shared<pi_t>(var,typ->quote(l),val->quote(l+1));
}
