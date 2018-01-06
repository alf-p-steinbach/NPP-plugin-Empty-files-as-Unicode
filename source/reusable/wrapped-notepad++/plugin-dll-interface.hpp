#pragma once
#include <wrapped-windows/windows_h.hpp>    // Before <PluginInterface.h> includes it.
#include <notepad++/PluginInterface.h>

namespace npp {
    using Notification_id = decltype( Sci_NotifyHeader::code );
}  // namespace npp
