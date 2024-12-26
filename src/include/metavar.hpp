#pragma once

// Header file for managing metavariables and postponed checks

#include "common.hpp"
#include "rsyntax.hpp"


// Metavariables
struct metaentry_t {

    value_ptr typ;

    metaentry_t(value_ptr typ) : typ{typ} {}
    virtual ~metaentry_t() {}

    virtual std::ostream& to_string(std::ostream&);
    virtual value_ptr get_value(value_ptr,spine_t&);
    virtual value_ptr get_value(std::size_t);
    virtual meta_ptr update(value_ptr);
    virtual std::pair<block_t,value_ptr> read_unsolved();
    virtual void add_block(std::size_t);
    virtual void unify(std::size_t,context_t&,term_ptr);

};
struct unsolvedmeta_t : metaentry_t {

    block_t checks;
    unsolvedmeta_t(value_ptr typ) : metaentry_t(typ), checks{block_t()} {}
    unsolvedmeta_t(value_ptr typ, block_t& checks) : metaentry_t(typ), checks{checks} {}

    std::ostream& to_string(std::ostream&);
    value_ptr get_value(value_ptr,spine_t&);
    value_ptr get_value(std::size_t);
    meta_ptr update(value_ptr);
    std::pair<block_t,value_ptr> read_unsolved();
    void add_block(std::size_t);
    void unify(std::size_t,context_t&,term_ptr);

};
struct solvedmeta_t : metaentry_t{

    value_ptr sol;

    solvedmeta_t(value_ptr typ, value_ptr sol) : metaentry_t(typ), sol{sol} {}

    std::ostream& to_string(std::ostream&);
    value_ptr get_value(value_ptr,spine_t&);
    value_ptr get_value(std::size_t);
    meta_ptr update(value_ptr);
    std::pair<block_t,value_ptr> read_unsolved();
    void add_block(std::size_t);
    void unify(std::size_t,context_t&,term_ptr);

};
std::ostream& operator<< (std::ostream&, metaentry_t&);

struct metavar_t {

    static metadata_t lookupTable;
    static meta_ptr lookup(std::size_t);

    std::size_t id;

    metavar_t(value_ptr typ) : id {lookupTable.size()} {lookupTable.push_back(std::make_shared<unsolvedmeta_t>(typ));}
    metavar_t(value_ptr typ,block_t checks) : id {lookupTable.size()} {lookupTable.push_back(std::make_shared<unsolvedmeta_t>(typ,checks));}


};

// Checks
struct checkentry_t {

    virtual ~checkentry_t() {}
    virtual term_ptr read();
    virtual void retry(std::size_t);

};
struct checked_t : checkentry_t {

    term_ptr res;
    checked_t(term_ptr res) : res{res} {}
    term_ptr read();
    void retry(std::size_t);

};
struct unchecked_t : checkentry_t {

    context_t cont;
    raw_ptr rterm;
    value_ptr typ;
    std::size_t meta;
    unchecked_t(context_t& cont, raw_ptr rterm, value_ptr typ, std::size_t meta) :
        cont{cont},
        rterm{rterm},
        typ{typ},
        meta{meta}
        {}
    term_ptr read();
    void retry(std::size_t);
};

struct check_t {

    static checkdata_t lookupTable;
    static check_ptr lookup(std::size_t);

    std::size_t id;

    check_t(context_t& cont, raw_ptr rterm, value_ptr typ, std::size_t meta) : 
        id {lookupTable.size()} 
        {lookupTable.push_back(std::make_shared<unchecked_t>(cont,rterm,typ,meta));}

};

struct renaming_t {

    std::size_t dom;
    std::size_t cod;
    renamingFun_t ren;
    std::optional<prunings_t> prune;

    renaming_t() : dom {0}, cod {0}, ren {renamingFun_t()}, prune{std::optional<prunings_t>()} {}
    renaming_t(prunings_t& prune) : dom {0}, cod {0}, ren {renamingFun_t()}, prune{prune} {}
    void lift();
    void skip();
    void pop();
    renaming_t(std::size_t,spine_t&);
};