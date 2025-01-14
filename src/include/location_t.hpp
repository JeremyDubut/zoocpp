#pragma once

// Header file for helping the error management in the lexer and the parser
// see https://stackoverflow.com/questions/76509844/how-do-you-interface-c-flex-with-c-bison

#include <cstddef>
#include <ostream>
#include <utility>

namespace foo
{
    using position_t = std::size_t;
    using location_t = std::pair<std::size_t, std::size_t>;

}

inline std::ostream& operator<<(std::ostream& os, const foo::location_t& loc)
{
    return os << "[" << loc.first << "-" << loc.second << "]";
}