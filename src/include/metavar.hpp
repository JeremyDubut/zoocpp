#pragma once

// Header file for managing metavariables

#include "common.hpp"

struct metaentry_t {

    value_ptr typ;

    metaentry_t(value_ptr typ) : typ{typ} {}
    virtual ~metaentry_t() {}

    virtual value_ptr get_value(value_ptr,spine_t&);
    virtual value_ptr get_value(std::size_t);
    virtual metaentry_t update(value_ptr);

};
struct solvedmeta_t : metaentry_t{

    value_ptr sol;

    solvedmeta_t(value_ptr typ, value_ptr sol) : metaentry_t(typ), sol{sol} {}

    value_ptr get_value(value_ptr,spine_t&);
    value_ptr get_value(std::size_t);
    metaentry_t update(value_ptr);

};

struct metavar_t {

    static metadata_t lookupTable;
    static metaentry_t lookup(std::size_t);

    std::size_t id;

    metavar_t(value_ptr typ) : id {lookupTable.size()} {lookupTable.push_back(metaentry_t(typ));}


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