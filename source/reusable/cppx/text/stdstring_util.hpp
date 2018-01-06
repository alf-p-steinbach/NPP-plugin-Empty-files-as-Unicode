#pragma once
#include <stdlib/extension/type_builders.hpp>

#include <stdlib/c/string.hpp>      // strlen
#include <stdlib/string.hpp>        // std::(wstring)

namespace cppx {
    using std::wstring;
    namespace stdlibx = stdlib;
    using namespace stdlib::ext::type_builders;

    inline auto wide_from_ascii( raw_array_<const char> s )
        -> wstring
    { return wstring( s, s + strlen( s ) ); }

}  // namespace cppx
