#pragma once

// Header file for managing metavariables

#include "common.hpp"

struct metaentry_t {

    value_ptr typ;

    metaentry_t(value_ptr typ) : typ{typ} {}
    virtual ~metaentry_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual value_ptr get_value(value_ptr,spine_t&);
    virtual value_ptr get_value(std::size_t);
    virtual meta_ptr update(value_ptr);

};
struct solvedmeta_t : metaentry_t{

    value_ptr sol;

    solvedmeta_t(value_ptr typ, value_ptr sol) : metaentry_t(typ), sol{sol} {}

    std::ostream& to_string(std::ostream&);
    value_ptr get_value(value_ptr,spine_t&);
    value_ptr get_value(std::size_t);
    meta_ptr update(value_ptr);

};
std::ostream& operator<< (std::ostream&, metaentry_t&);

struct metavar_t {

    static metadata_t lookupTable;
    static meta_ptr lookup(std::size_t);

    std::size_t id;

    metavar_t(value_ptr typ) : id {lookupTable.size()} {lookupTable.push_back(std::make_shared<metaentry_t>(typ));}


};

struct renaming_t {

    std::size_t dom;
    std::size_t cod;
    renamingFun_t ren;
    std::optional<prunings_t> prune;

    renaming_t() : dom {0}, cod {0}, ren {renamingFun_t()}, prune{std::optional<prunings_t>()} {}
    void lift();
    void skip();
    void pop();
    renaming_t(std::size_t,spine_t&);
};