#include "metavar.hpp"
#include <sstream>
#include <iostream>
#include "value.hpp"
#include "syntax.hpp"

value_ptr metaentry_t::get_value(value_ptr, spine_t&) {
    throw "Getting a value out of an unknown metavariable";
}
value_ptr unsolvedmeta_t::get_value(value_ptr v, spine_t&) {return v;}
value_ptr solvedmeta_t::get_value(value_ptr, spine_t& spine) {
    return sol->clone()->vAppSp(spine)->force();
}
value_ptr metaentry_t::get_value(std::size_t) {
   throw "Getting a value out of an unknown metavariable";
}
value_ptr unsolvedmeta_t::get_value(std::size_t index) {
    return std::make_shared<vflex_t>(index);
    }
value_ptr solvedmeta_t::get_value(std::size_t) {
    return sol;}

meta_ptr metaentry_t::update(value_ptr) {
    throw "Updating an unknown metaentry";
}
meta_ptr unsolvedmeta_t::update(value_ptr sol) {
    LOG("Updating value of unsolved metavariable to " << *sol);
    return std::make_shared<solvedmeta_t>(typ,sol);
}
meta_ptr solvedmeta_t::update(value_ptr newsol) {
    LOG("Updating value of an already solved metavariable from " << *sol << " to " << *newsol);
    return std::make_shared<solvedmeta_t>(typ,newsol);
}
value_ptr metaentry_t::get_sol() {
    throw "Getting solution of an unsolved metavariable";
}
value_ptr solvedmeta_t::get_sol() {
    return sol;
}
raw_ptr metaentry_t::display(names_t&) {
    return std::make_shared<rhole_t>();
}
raw_ptr solvedmeta_t::display(names_t& names) {
    return sol->quote(0)->nf()->display_rec(names);
}

metadata_t metavar_t::lookupTable {};
meta_ptr metavar_t::lookup(std::size_t i) {
    if (i < lookupTable.size()) {
        return lookupTable[i];
    }
    else {
        std::stringstream ss("");
        ss << "Lookup error: non-existing metavariable " << i;
        throw ss.str();
    }
}

checkdata_t check_t::lookupTable {};
check_ptr check_t::lookup(std::size_t i) {
    if (i < lookupTable.size()) {
        return lookupTable[i];
    }
    else {
        std::stringstream ss("");
        ss << "Lookup error: non-existing check " << i;
        throw ss.str();
    }
}
std::size_t check_t::done_check = 0;

void renaming_t::lift() {
    ren[cod] = dom;
    dom++;
    cod++;
}
void renaming_t::skip() {
    cod++;
}
void renaming_t::pop() {
    dom--;
    cod--;
    ren.erase(cod);
}

renaming_t::renaming_t(std::size_t l, spine_t& spine) {
    ren = renamingFun_t();
    cod = l;
    dom = 0;
    prunings_t prune_val {};
    std::unordered_set<std::size_t> nlvars {};
    std::vector<std::pair<std::size_t,bool>> fsp {};
    for (auto it : spine) {
        std::size_t x = it.first->clone()->force()->inverse();
        if (ren.contains(x) || nlvars.contains(x)) {
            dom++;
            ren.erase(x);
            nlvars.insert(x);
            fsp.push_back(std::make_pair(x,it.second));
        }
        else {
            ren[x] = dom;
            dom++;
            fsp.push_back(std::make_pair(x,it.second));
        }
    }
    for (auto it2 : fsp) {
        if (nlvars.contains(it2.first)) {
            prune_val.push_back(None);
        }
        else if (it2.second) {
            prune_val.push_back(Implicit);
        }
        else {
            prune_val.push_back(Explicit);
        }
    }
    if (nlvars.empty()) {
        prune = std::optional<prunings_t>();
    }
    else {
        prune = std::optional<prunings_t>(prune_val);
    }
}

std::ostream& metaentry_t::to_string(std::ostream& out) {
    return out << "Unknown metava";
}
std::ostream& unsolvedmeta_t::to_string(std::ostream& out) {
    return out << "Not solved metavar";
}
std::ostream& solvedmeta_t::to_string(std::ostream& out) {
    return out << "Solved metavar with " << *sol;
}
std::ostream& operator<< (std::ostream& out, metaentry_t& m) {
    return m.to_string(out);
}

std::pair<block_t,value_ptr> metaentry_t::read_unsolved() {
    throw "Reading unknown metavariable";
}
std::pair<block_t,value_ptr> unsolvedmeta_t::read_unsolved() {
    return std::make_pair(checks,typ);
}
std::pair<block_t,value_ptr> solvedmeta_t::read_unsolved() {
    throw "Unification error: trying to prune solved metavariable";
}

void metaentry_t::add_block(std::size_t) {
    throw "Adding a blocked check in an unknown metavariable";
}
void solvedmeta_t::add_block(std::size_t) {
    throw "Unification error: a solved metavariable cannot block a check";
}
void unsolvedmeta_t::add_block(std::size_t index) {
    checks.insert(index);
}

void metaentry_t::unify(std::size_t,context_t&,term_ptr) {
    throw "Unifiying an unknown metavariable";
}
void unsolvedmeta_t::unify(std::size_t m, context_t& cont,term_ptr t) { 
    LOG("Unifying unsolved meta");
    term_ptr sol = cont.local->closetm(t);
    metavar_t::lookupTable[m] = update(sol->eval()); 
    for (auto it : checks) {
        check_t::lookup(it)->retry(it);
    }
}
void solvedmeta_t::unify(std::size_t m, context_t& cont,term_ptr t) { 
    LOG("Unifying solved meta " << m << " with solution " << *sol << " with term " << *t);
    if (cont.environment.size() != cont.prune.size()) {
        std::stringstream ss("");
        ss << "Unification error: Inconsistency between environments "; 
        ss << cont.environment.size() << " and prunings " << cont.prune.size();
        throw ss.str();
    }
    auto ite = cont.environment.begin();
    value_ptr res = sol;
    for (pruning_t itf : cont.prune) {
        switch (itf) {
            case Implicit: {
                LOG("Imp case with " << **ite); 
                res =res->vApp(*ite,true); 
                break;
            }
            case Explicit: {
                LOG("Exp case with " << **ite); 
                res = res->vApp(*ite,false); 
                break;
            }
            case None: {
                LOG("None case");
                break;
            }
        }
        ite++;
        LOG("After beta-red: " << *res);
    }
    t->eval(cont.environment)->force()->unify(cont.level,res);
}

term_ptr checkentry_t::read() {
    throw "Reading an unknown checkentry";
}
term_ptr checked_t::read() {
    return res;
}
term_ptr unchecked_t::read() {
    return std::make_shared<appp_t>(std::make_shared<meta_t>(meta),cont.prune);
}

void checkentry_t::retry(std::size_t) {
    throw "Retrying an unknown check";
}
void checked_t::retry(std::size_t) {}
void unchecked_t::retry(std::size_t c) {
    typ->force()->retry(c,cont,rterm,meta);
}
void checkentry_t::final(std::size_t) {
    throw "Final checking an unknown check";
}
void checked_t::final(std::size_t) {
    LOG("Check already checked");
}
void unchecked_t::final(std::size_t c) {
    LOG("Check not checked yet");
    inferrance_t inf = rterm->infer(cont);
    inf = inf.term->insert(cont,inf.typ);
    LOG("Inferrance of the check done");
    check_t::lookupTable[c] = std::make_shared<checked_t>(inf.term);
    typ->force()->unify(cont.level,inf.typ);
    LOG("Unification of types done");
    metavar_t::lookup(meta)->unify(meta,cont,inf.term);
}

std::ostream& checkentry_t::to_string(std::ostream& out) {
    return out << "unknown check";
}
std::ostream& checked_t::to_string(std::ostream& out) {
    return out << "already checked";
}
std::ostream& unchecked_t::to_string(std::ostream& out) {
    return out << "unchecked with meta " << meta;
}
std::ostream& operator<< (std::ostream &out, checkentry_t &c) {
    return c.to_string(out);
}