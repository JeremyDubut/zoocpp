#pragma once

// Header file for managing metavariables

#include "common.hpp"


struct metavar_t {

    static metadata_t lookupTable;
    static metaentry_t lookup(std::size_t);

    std::size_t id;

    metavar_t() : id {lookupTable.size()} {lookupTable.push_back(std::optional<value_ptr>());}


};

struct renaming_t {

    std::size_t dom;
    std::size_t cod;
    renamingFun_t ren;

    renaming_t() : dom {0}, cod {0}, ren {renamingFun_t()} {}
    void lift();
    void pop();
    renaming_t(std::size_t,spine_t&);
};