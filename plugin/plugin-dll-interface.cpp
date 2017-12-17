#include "Plugin.hpp"

#include <notepad++/plugin-dll-interface.hpp>
#include <stdlib/extension/type_builders.hpp>
using namespace stdlib::type_builders;

#include <memory>
using std::unique_ptr;

namespace npp {
    using Handles       = ::NppData;
    using Menu_item     = ::FuncItem;
}  // namespace npp

namespace {
    unique_ptr<Plugin> plugin_singleton;

    raw_array_<npp::Menu_item> menu_items =
    {
        { L"About…", []{ plugin_singleton->cmd_about(); }, 0, false, nullptr },
#ifndef NDEBUG
        {},
        { L"Enabled", []{}, 0, true, nullptr },
        { L"Disabled", []{}, 0, false, nullptr },
        {},
        { L"Check all", []{ plugin_singleton->cmd_check_all(); }, 0, false, nullptr },
        { L"Show doc info…", []{ plugin_singleton->cmd_show_doc_info(); }, 0, false, nullptr },
#endif
    };
}  // namespace <anon>

#define EXPORT __declspec( dllexport )
extern "C" {
    EXPORT void setInfo( const npp::Handles handles )
    {
        if( plugin_singleton != nullptr ) { return; }     // TODO: log the problem
        plugin_singleton = std::make_unique<Plugin>( handles );
    }

    EXPORT auto getName() -> ptr_<const wchar_t> { return Plugin::name; }

    EXPORT auto getFuncsArray( const ptr_<int> n_funcs )
        -> ptr_<FuncItem>
    {
	    *n_funcs = static_cast<int>( std::size( menu_items ) );
	    return &menu_items[0];
    }

    EXPORT void beNotified( const ptr_<SCNotification> p_notification )
    {
        if( plugin_singleton == nullptr ) { return; }   // TODO: log the problem
        plugin_singleton->on_notification(
            p_notification->nmhdr.code,
            static_cast<npp::Buffer_id::Enum>( p_notification->nmhdr.idFrom )   // Undocumented.
            );
    }

    EXPORT auto messageProc(
        const UINT              message_id,
        const WPARAM            w_param,
        const LPARAM            l_param
        )
        -> LRESULT
    {
        (void) message_id; struct message_id; (void) w_param; struct w_param; (void) l_param; struct l_param;
        return true;
    }

    EXPORT auto isUnicode() -> BOOL { return true; }
}  // extern "C"
