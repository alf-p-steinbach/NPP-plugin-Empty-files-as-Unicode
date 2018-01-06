#pragma once
#include <tinyxml2/tinyxml2.h>
#include <stdlib/extension/hopefully_and_fail.hpp>

namespace tinyxml2 { // extension
    using namespace stdlib::ext::hopefully_and_fail;        // Success

    inline auto operator>>( XMLError const e, Success )
        -> bool
    { return (e == XML_SUCCESS); }

}  // namespace tinyxml2
