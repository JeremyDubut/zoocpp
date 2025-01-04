#pragma once

#include "common.hpp"

struct myexception : std::exception{
    virtual std::ostream& mywhat(std::ostream&) const noexcept;
};
std::ostream& operator<< (std::ostream&,myexception&);
struct unification_e : myexception {};
struct inverse_e : unification_e {};
struct non_var_e : inverse_e {
    value_ptr non_var;
    non_var_e(const value_ptr non_var) : non_var{non_var} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct non_empty_spine_e : inverse_e {
    std::size_t level, size;
    non_empty_spine_e(const std::size_t level, const std::size_t size) : 
        level{level}, size{size} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct wrap_e : unification_e {
    std::size_t level;
    value_ptr typ;
    wrap_e(const std::size_t level) : level{level} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct rename_e : unification_e {
    value_ptr unknown;
    rename_e(const value_ptr unknown) : unknown{unknown} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct occurs_check_e : unification_e {
    std::size_t index;
    occurs_check_e(const std::size_t index) : index{index} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct rename_escape_rig_var_e : unification_e {
    std::size_t level;
    rename_escape_rig_var_e(const std::size_t level) : level{level} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct unknown_unification_e : unification_e {
    value_ptr unknown;
    unknown_unification_e(const value_ptr unknown) : unknown{unknown} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct rigid_u_e : unification_e {
    value_ptr unknown;
    rigid_u_e(const value_ptr unknown) : unknown{unknown} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct rigid_pi_e : unification_e {
    value_ptr unknown;
    rigid_pi_e(const value_ptr unknown) : unknown{unknown} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct icit_pi_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct rigid_ipi_e : unification_e {
    value_ptr unknown;
    rigid_ipi_e(const value_ptr unknown) : unknown{unknown} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct rigid_rig_e : unification_e {
    value_ptr unknown;
    rigid_rig_e(const value_ptr unknown) : unknown{unknown} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct rigid_rig_rig_e : unification_e {
    std::size_t level1, level2;
    rigid_rig_rig_e(const std::size_t level1,const std::size_t level2) 
        : level1{level1}, level2{level2} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct escape_flex_var_e : unification_e {
    std::size_t index;
    escape_flex_var_e(const std::size_t index) : index{index} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct intersect_e : unification_e {
    std::size_t size1, size2;
    intersect_e(const std::size_t size1,const std::size_t size2) 
        : size1{size1}, size2{size2} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct get_val_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct meta_lookup_e : unification_e {
    std::size_t index;
    meta_lookup_e(const std::size_t index) : index{index} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct check_lookup_e : unification_e {
    std::size_t index;
    check_lookup_e(const std::size_t index) : index{index} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct read_unknown_meta_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct read_solved_meta_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct block_unknown_meta_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct block_solved_meta_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct unify_unknown_meta_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct unify_solved_inconsistent_e : unification_e {
    std::size_t size1, size2;
    unify_solved_inconsistent_e(const std::size_t size1,const std::size_t size2) 
        : size1{size1}, size2{size2} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct read_unknown_check_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct retry_unknown_check_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct final_unknown_check_e : unification_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct prune_e : myexception {};
struct prune_non_var_e : prune_e {
    value_ptr non_var;
    prune_non_var_e(const value_ptr non_var) : non_var{non_var} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct prune_non_pi_e : prune_e {
    value_ptr non_pi;
    prune_non_pi_e(const value_ptr non_pi) : non_pi{non_pi} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct spine_not_renaming_e : prune_e {
    std::size_t level;
    spine_not_renaming_e(const std::size_t level) : level{level} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct prune_non_empty_spine_e : prune_e {
    std::size_t level, size;
    prune_non_empty_spine_e(const std::size_t level,const std::size_t size) 
        : level{level}, size{size} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct escape_rig_var_e : prune_e {
    std::size_t level;
    escape_rig_var_e(const std::size_t level) : level{level} {};
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct beta_red_e : myexception {
    value_ptr non_fun;
    beta_red_e(const value_ptr non_fun) : non_fun{non_fun} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct reification_e : myexception {
    value_ptr unknown;
    reification_e(const value_ptr unknown) : unknown{unknown} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct check_e : myexception {};
struct unbound_var_e : check_e {
    name_t var;
    unbound_var_e(const name_t& var) : var{var} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct inferrance_e : myexception {};
struct icit_mismatch_ei_e : inferrance_e {
    value_ptr ipi;
    icit_mismatch_ei_e(const value_ptr ipi) : ipi{ipi} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct icit_mismatch_ie_e : inferrance_e {
    value_ptr epi;
    icit_mismatch_ie_e(const value_ptr epi) : epi{epi} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct no_named_icit_arg_e : inferrance_e {
    name_t ivar;
    value_ptr val;
    no_named_icit_arg_e(const name_t& ivar, const value_ptr val) : 
        ivar{ivar}, val{val} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct infer_unknown_e : inferrance_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct infer_named_imp_lam_e : inferrance_e {
    raw_ptr lam;
    infer_named_imp_lam_e(const raw_ptr lam) : lam{lam} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct infer_unbound_var_e : check_e {
    name_t var;
    infer_unbound_var_e(const name_t& var) : var{var} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct evaluation_e : myexception {};
struct eval_unknown_e : evaluation_e {
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct eval_appp_inconsistent_e : evaluation_e {
    std::size_t size1, size2;
    eval_appp_inconsistent_e(const std::size_t size1,const std::size_t size2) 
        : size1{size1}, size2{size2} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct parser_e : myexception {};
struct build_e : parser_e {
    raw_ptr non_util;
    build_e(const raw_ptr non_util) : non_util{non_util} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct auto_build_e : parser_e {
    raw_ptr non_list;
    auto_build_e(const raw_ptr non_list) : non_list{non_list} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct pushback_e : parser_e {
    raw_ptr non_list;
    pushback_e(const raw_ptr non_list) : non_list{non_list} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};
struct namelist_e : parser_e {
    raw_ptr non_list;
    namelist_e(const raw_ptr non_list) : non_list{non_list} {}
    std::ostream& mywhat(std::ostream&) const noexcept;
};