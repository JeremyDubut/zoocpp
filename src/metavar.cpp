#include "metavar.hpp"
#include <sstream>
#include <iostream>
#include "value.hpp"

value_ptr metaentry_t::get_value(value_ptr v, spine_t&) {return v;}
value_ptr solvedmeta_t::get_value(value_ptr, spine_t& spine) {
    return sol->clone()->vAppSp(spine)->force();
}
value_ptr metaentry_t::get_value(std::size_t index) {
    return std::make_shared<vflex_t>(index);
    }
value_ptr solvedmeta_t::get_value(std::size_t) {
    return sol;}

meta_ptr metaentry_t::update(value_ptr sol) {
    LOG("Updatin value of unsolved metavariable to " << *sol);
    return std::make_shared<solvedmeta_t>(typ,sol);
}
meta_ptr solvedmeta_t::update(value_ptr newsol) {
    LOG("Updating value of an already solved metavariable from " << *sol << " to " << *newsol);
    return std::make_shared<solvedmeta_t>(typ,newsol);
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
    auto it = spine.end()-1;
    while (it != spine.begin()-1) {
        std::size_t x = it->first->clone()->force()->inverse();
        if (ren.contains(x) || nlvars.contains(x)) {
            dom++;
            ren.erase(x);
            nlvars.insert(x);
            fsp.push_back(std::make_pair(x,it->second));
        }
        else {
            ren[x] = dom;
            dom++;
            fsp.push_back(std::make_pair(x,it->second));
        }
        it = it-1;
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
    prune = std::optional<prunings_t>(prune_val);
}

std::ostream& metaentry_t::to_string(std::ostream& out) {
    return out << "Not solved metavar";
}
std::ostream& solvedmeta_t::to_string(std::ostream& out) {
    return out << "Solved metavar with " << *sol;
}
std::ostream& operator<< (std::ostream& out, metaentry_t& m) {
    return m.to_string(out);
}