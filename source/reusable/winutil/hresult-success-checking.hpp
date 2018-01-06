#pragma once

#include <stdlib/extension/hopefully_and_fail.hpp>      // stdlib::ext::(Success)
#include <wrapped-windows/windows_h.hpp>

namespace winutil {
    using stdlib::ext::Success;

    inline auto operator>>( ::HRESULT const hr, Success )
        -> bool
    { return SUCCEEDED( hr ); }

}  // namespace winutil
