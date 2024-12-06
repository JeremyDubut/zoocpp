#pragma once
#include "common.hpp"


struct metavar_t {

    static metadata_t lookupTable;
    static metaentry_t lookup(std::size_t);

    std::size_t id;

    metavar_t() : id {lookupTable.size()} {lookupTable.push_back(std::optional<value_ptr>());}


};