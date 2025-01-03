#include "errors.hpp"
#include "value.hpp"


std::ostream& operator<< (std::ostream& out, myexception& e) {
    return e.mywhat(out);
}
std::ostream& myexception::mywhat(std::ostream& out) const noexcept {
    return out;
}
std::ostream& non_var_e::mywhat(std::ostream& out) const noexcept {
    return out << "Non-variable " << *non_var << " in inverse";
}
std::ostream& non_empty_spine_e::mywhat(std::ostream& out) const noexcept {
    out << "Variable " << level << " with non-empty spine of size ";
    return out << size << " in inverse";
}
std::ostream& beta_red_e::mywhat(std::ostream& out) const noexcept {
    out << "Apply to a term that is not a lambda or a metavariable ";
    return out << *non_fun;
}
std::ostream& reification_e::mywhat(std::ostream& out) const noexcept {
    out << "Reify unknown value ";
    return out << *unknown;
}
std::ostream& unbound_var_e::mywhat(std::ostream& out) const noexcept {
    return out << "Unbound variable " << var;
}
std::ostream& icit_mismatch_ei_e::mywhat(std::ostream& out) const noexcept {
    return out << "Mismatch explicit application vs implicit pi " << ipi;
}
std::ostream& icit_mismatch_ie_e::mywhat(std::ostream& out) const noexcept {
    return out << "Mismatch implicit application vs explicit pi " << epi;
}
std::ostream& no_named_icit_arg_e::mywhat(std::ostream& out) const noexcept {
    return out << "No named implicit argument " << ivar << " in " << *val;
}
std::ostream& wrap_e::mywhat(std::ostream& out) const noexcept {
    return out << "Impossible to wrap lambdas at level " << level << " with type " << *typ;
}
std::ostream& prune_non_var_e::mywhat(std::ostream& out) const noexcept {
    return out << "Prune non variable value " << *non_var;
}
std::ostream& spine_not_renaming_e::mywhat(std::ostream& out) const noexcept {
    return out << "Spine of rigid variable " << level << " is not a renaming";
}
std::ostream& prune_non_empty_spine_e::mywhat(std::ostream& out) const noexcept {
    return out << "Prune a rigid variable " << level << " with a non-empty spine of size " << size;
}
std::ostream& escape_rig_var_e::mywhat(std::ostream& out) const noexcept {
    return out << "Prune an escaped rigid variable " << level;
}
std::ostream& rename_e::mywhat(std::ostream& out) const noexcept {
    return out << "Rename an unknown value " << unknown;
}
std::ostream& occurs_check_e::mywhat(std::ostream& out) const noexcept {
    return out << "Occurs check of flexible variable " << index;
}
std::ostream& rename_escape_rig_var_e::mywhat(std::ostream& out) const noexcept {
    return out << "Rename an escaped rigid variable " << level;
}
std::ostream& unknown_unification_e::mywhat(std::ostream& out) const noexcept {
    return out << "Unify an unknown value " << unknown;
}
std::ostream& rigid_u_e::mywhat(std::ostream& out) const noexcept {
    return out << "Rigid mismatch between 𝒰 and " << unknown;
}
std::ostream& rigid_pi_e::mywhat(std::ostream& out) const noexcept {
    return out << "Rigid mismatch between explicit pi and " << unknown;
}
std::ostream& icit_pi_e::mywhat(std::ostream& out) const noexcept {
    return out << "Icit mismatch in unifying pis";
}
std::ostream& rigid_ipi_e::mywhat(std::ostream& out) const noexcept {
    return out << "Rigid mismatch between implicit pi and " << unknown;
}
std::ostream& rigid_rig_e::mywhat(std::ostream& out) const noexcept {
    return out << "Rigid mismatch between rigid variable and " << unknown;
}
std::ostream& rigid_rig_rig_e::mywhat(std::ostream& out) const noexcept {
    return out << "Rigid mismatch between rigid variables " << level1 << " and " << level2;
}
std::ostream& escape_flex_var_e::mywhat(std::ostream& out) const noexcept {
    return out << "Unify with an escaped rigid variable " << index;
}
std::ostream& intersect_e::mywhat(std::ostream& out) const noexcept {
    return out << "Mismatch between intersected spines of sizes " << size1 << " and " << size2;
}
std::ostream& prune_non_pi_e::mywhat(std::ostream& out) const noexcept {
    return out << "Prune non variable value " << *non_pi;
}