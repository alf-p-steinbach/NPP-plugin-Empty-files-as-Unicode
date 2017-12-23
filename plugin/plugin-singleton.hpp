#pragma once

#include "Plugin.hpp"
#include <stdlib/extension/type_builders.hpp>
#include <memory>           // std::unique_ptr

namespace plugin {
    using std::unique_ptr;
    using namespace stdlib::type_builders;      // ref_

    // The singleton is created by DLL func setInfo() in "plugin-dll-interface.cpp".
    // It's (also) accessed by the menu data initialization in "menu.cpp".
    //
    // DLL funcs            use plugin singleton
    // DLL funcs            use menu data
    // plugin singleton     uses menu data (namely Notepad++'s dynamic command ids)
    //
    // And the menu data implementation uses the plugin singleton, as event handler.


    namespace impl{
        inline auto singleton_pointer()
            -> ref_<unique_ptr<Plugin>>
        {
            static unique_ptr<Plugin> the_pointer;
            return the_pointer;
        }
    }  // namespace impl

    inline auto singleton()
        -> ref_<Plugin>
    { return *impl::singleton_pointer(); }

}  // namespace plugin
