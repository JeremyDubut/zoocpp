#include "metavar.hpp"
#include <sstream>
#include "value.hpp"

metadata_t metavar_t::lookupTable {};
metaentry_t metavar_t::lookup(std::size_t i) {
    if (i < lookupTable.size()) {
        return lookupTable[i];
    }
    else {
        std::stringstream ss("");
        ss << "Looking up to a non-existing metavariable " << i;
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
    for (value_ptr it : spine) {
        std::size_t x = it->clone()->force()->inverse();
        if (!ren.contains(x)) {
            ren[x] = dom;
            dom++;
        }
        else {
            throw "Unification error: several occurences of a variable in inverse";
        }
    }
}