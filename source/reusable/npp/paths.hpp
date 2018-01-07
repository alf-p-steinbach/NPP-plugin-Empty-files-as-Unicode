#pragma once
#include <npp/appname.hpp>              // npp::appname
#include <stdlib/extension/type_builders.hpp>
#include <stdlib/filesystem.hpp>        // std::filesystem::*
#include <winutil/known-paths.hpp>      // winutil::*

namespace npp{
    namespace fs = std::filesystem;
    using namespace stdlib::ext::type_builders;

    using Path = fs::path;

    struct Paths
    {
        virtual auto installation_folder() const
            -> Path
        { return winutil::path_to_executable().parent_path(); }

        virtual auto config_folder() const
            -> Path
        {
            const Path installation_folder_path = installation_folder();
            return (
                fs::exists( installation_folder_path / L"doLocalConf.xml" )
                    ? installation_folder_path
                    : winutil::path_to_appdata() / appname
                );
        }

        virtual auto gui_config_file() const
            -> Path
        { return config_folder() / L"config.xml"; }
    };

}  // namespace hpp
