// see https://stackoverflow.com/questions/76509844/how-do-you-interface-c-flex-with-c-bison

#pragma once

#include <cstddef>
#include <ostream>
#include <utility>
#include "common.hpp"
#include "rsyntax.hpp"

namespace foo
{
    using position_t = std::size_t;
    using location_t = std::pair<std::size_t, std::size_t>;

    struct pibinder_t {
        std::vector<name_t> names;
        raw_ptr typ;
        bool icit;

        pibinder_t(std::vector<name_t> names, raw_ptr typ, bool icit): names {names}, typ {typ}, icit {icit} {}
    };

    raw_ptr pifoldr(std::vector<pibinder_t>& v, raw_ptr r) {
        if (v.empty()) {
            return r;
        }
        pibinder_t pb = *(v.end()-1);
        v.pop_back();
        raw_ptr res = pifoldr(v,r);
        while (!pb.names.empty()) {
            name_t var = *(pb.names.end()-1);
            pb.names.pop_back();
            if (pb.icit) {
                res = std::make_shared<ripi_t>(var,pb.typ,res);
            }
            else {
                res = std::make_shared<rpi_t>(var,pb.typ,res);
            }
        }
        return res;
    }

    struct icit {
        name_t bind;
        icit(name_t bind) : bind{bind} {}
        virtual raw_ptr rptr(raw_ptr) {
            throw "Wrong type of icit";
        }
    };
    struct iicit : icit {
        iicit(name_t bind) : icit(bind) {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<riabs_t>(bind,r);
        }
    };
    struct eicit : icit {
        eicit(name_t bind) : icit(bind) {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<rabs_t>(bind,r);
        }
    };
    struct nicit : icit {
        name_t name;
        nicit(name_t bind,name_t name): icit(bind), name{name} {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<rnabs_t>(bind,r,name);
        }
    };

    raw_ptr lamfoldr(std::vector<icit>& v, raw_ptr r) {
        raw_ptr res = r;
        while (!v.empty()) {
            icit d = *(v.end()-1);
            res = d.rptr(res);
        }
        return res;
    }


    struct arg_t {
        raw_ptr arg;
        arg_t(raw_ptr arg) : arg{arg} {}
        virtual raw_ptr rptr(raw_ptr) {
            throw "Wrong type of icit";
        }
    };
    struct iarg_t : arg_t {
        iarg_t(raw_ptr arg) : arg_t(arg) {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<riapp_t>(r,arg);
        }
    };
    struct earg_t : arg_t {
        earg_t(raw_ptr arg) : arg_t(arg) {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<rapp_t>(r,arg);
        }
    };
    struct narg_t : arg_t {
        name_t name;
        narg_t(raw_ptr arg,name_t name): arg_t(arg), name{name} {}
        raw_ptr rptr(raw_ptr r) {
            return std::make_shared<rnapp_t>(r,arg,name);
        }
    };

    raw_ptr appfoldl(raw_ptr r, std::vector<arg_t>& v) {
        raw_ptr res = r;
        for (arg_t arg : v) {
            res = arg.rptr(r);
        }
        return res;
    }
}

inline std::ostream& operator<<(std::ostream& os, const foo::location_t& loc)
{
    return os << "[" << loc.first << "-" << loc.second << "]";
}