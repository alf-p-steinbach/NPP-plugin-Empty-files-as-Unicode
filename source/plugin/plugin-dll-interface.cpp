#include <wrapped-notepad++/plugin-dll-interface.hpp>

#include "menu.hpp"
#include "Plugin.hpp"
#include "plugin-singleton.hpp"

#include <stdlib/extension/type_builders.hpp>
using namespace stdlib::ext::type_builders;

#include <stdlib/c/assert.hpp>      // assert
#include <stdlib/memory.hpp>        // std::unique_ptr
using std::unique_ptr;

void dbginfo( ref_<const std::wstring> s )
{
    MessageBox( 0, s.c_str(), L"DbgInfo:", MB_ICONINFORMATION | MB_SETFOREGROUND );
}

namespace npp {
    using Handles       = ::NppData;
}  // namespace npp

#define EXPORT __declspec( dllexport )
extern "C" {
    EXPORT void setInfo( const npp::Handles handles )
    {
        CPPX_DBGINFO( L"setInfo()" );
        auto& ps = plugin::impl::singleton_pointer;
        if( ps() != nullptr ) { return; }       // TODO: log the problem
        ps() = std::make_unique<Plugin>( handles );
    }

    EXPORT auto getName()
        -> ptr_<const wchar_t>
    {
        return Plugin::name;
    }

    EXPORT auto getFuncsArray( const ptr_<int> n_funcs )
        -> ptr_<FuncItem>
    {
        CPPX_DBGINFO( L"getFuncsArray()" );
        const menu::Items items = menu::items();
        *n_funcs = items.count;
        return &items.item[0];
    }

    EXPORT void beNotified( const ptr_<SCNotification> p_notification )
    {
        if( plugin::impl::singleton_pointer() == nullptr )
        {
            return;         // TODO: log the problem
        }

        plugin::singleton().on_notification(
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
