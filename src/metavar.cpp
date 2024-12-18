#include "metavar.hpp"
#include <sstream>
#include <iostream>
#include "value.hpp"

value_ptr metaentry_t::get_value(value_ptr v, spine_t&) {return v;}
value_ptr solvedmeta_t::get_value(value_ptr, spine_t& spine) {
    return sol->clone()->vAppSp(spine)->force();
}
value_ptr metaentry_t::get_value(std::size_t index) {return std::make_shared<vflex_t>(index);}
value_ptr solvedmeta_t::get_value(std::size_t) {return sol;}

metaentry_t metaentry_t::update(value_ptr sol) {
    return solvedmeta_t(typ,sol);
}
metaentry_t solvedmeta_t::update(value_ptr) {
    throw "Unification error: trying to solve an already solved metavariable";
}

metadata_t metavar_t::lookupTable {};
metaentry_t metavar_t::lookup(std::size_t i) {
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
void renaming_t::pop() {
    dom--;
    cod--;
    ren.erase(cod);
}

renaming_t::renaming_t(std::size_t l, spine_t& spine) {
    ren = renamingFun_t();
    cod = l;
    dom = 0;
    for (auto it : spine) {
        std::size_t x = it.first->clone()->force()->inverse();
        if (!ren.contains(x)) {
            ren[x] = dom;
            dom++;
        }
        else {
            throw "Unification error: several occurences of a variable in inverse";
        }
    }
}