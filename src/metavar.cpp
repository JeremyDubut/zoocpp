#include "metavar.hpp"
#include <sstream>

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