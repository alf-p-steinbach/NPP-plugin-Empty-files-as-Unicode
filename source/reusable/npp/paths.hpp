#pragma once
#include <npp/appname.hpp>              // npp::appname
#include <stdlib/extension/type_builders.hpp>
#include <stdlib/filesystem.hpp>        // std::filesystem::*
#include <winutil/known-paths.hpp>      // winutil::*

namespace npp{
    namespace fs = std::filesystem;
    using namespace stdlib::ext::type_builders;

    using Path = fs::path;

    inline auto path_to_installation()
        -> Path
    {
        return winutil::path_to_executable().parent_path();
    }

    inline auto path_to_config_folder(
        ref_<const Path> installation_path      = path_to_installation()
        )
        -> Path
    {
        return (
            fs::exists( installation_path / L"doLocalConf.xml" )
                ? installation_path
                : winutil::path_to_appdata() / appname
            );
    }

    inline auto path_to_gui_config(
        ref_<const Path> config_folder_path     = path_to_config_folder()
        )
        -> Path
    {
        return config_folder_path / L"config.xml";
    }

}  // namespace hpp
