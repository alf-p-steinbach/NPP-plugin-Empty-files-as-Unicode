#pragma once
#include <cppx/Scope_guard.hpp>                         // cppx::Scope_guard
#include <stdlib/extension/hopefully_and_fail.hpp>      // stdlib::ex::(hopefully, fail)
#include <stdlib/extension/type_builders.hpp>           // stdlib::ex::ref_
#include <winutil/hresult-success-checking.hpp>         // winutil::operator>>
#include <winutil/registry.hpp>                         // winutil::reg::*
#include <wrapped-windows/shlobj_h.hpp>                 // ::SHGetKnownFolderPath, etc.

#include <stdlib/filesystem.hpp>    // std::filesystem::*
#include <stdlib/string.hpp>        // std::wstring

namespace winutil {
    using cppx::Scope_guard;
    using std::wstring;

    using namespace stdlib::ext::hopefully_and_fail;
    using namespace stdlib::ext::type_builders;

    namespace fs = std::filesystem;

    using Path = fs::path;

    // Summary:
    inline auto path_to_known_folder( ref_<const ::KNOWNFOLDERID> id ) -> Path;
    inline auto path_to_program_files_32() -> Path;
    inline auto path_to_program_files_64() -> Path;
    inline auto path_to_appdata() -> Path;
    inline auto path_to_executable() -> Path;

    inline auto path_to_known_folder( ref_<const ::KNOWNFOLDERID> id )
        -> Path
    {
        wchar_t* p_path = nullptr;
        ::SHGetKnownFolderPath(
            id,
            0,                      // dwFlags,
            0,                      // hToken,
            &p_path
            ) >> Success{}
            or fail( "winutil::path_to_known_folder - SHGetKnownFolderPath failed" );
        const Scope_guard p_path_deallocation = [p_path]{ ::CoTaskMemFree( p_path ); };
        return p_path;
    }

    inline auto path_to_program_files_32()
        -> Path
    { return path_to_known_folder( ::FOLDERID_ProgramFilesX86 ); }

    inline auto path_to_program_files_64()
        -> Path
    {
        const ::HKEY key = reg::open_subkey_for_read(
            reg::hklm, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
            );
        const Scope_guard key_closing = [key]{ reg::close( key ); };
        return reg::string_value( key, L"ProgramW6432Dir" );
    }

    inline auto path_to_appdata()
        -> Path
    { return path_to_known_folder( ::FOLDERID_RoamingAppData ); }

    inline auto path_to_executable()
        -> Path
    {
        wstring result( MAX_PATH, L'\0' );
        ::SetLastError( 0 );
        int n_nonnull_chars = ::GetModuleFileName( 0, &result[0], static_cast<DWORD>( result.size() ) );
        hopefully( ::GetLastError() == 0 )
            or fail( "winutil::path_of_executable - GetModuleFileName failed" );
        result.resize( n_nonnull_chars );
        return result;
    }

}  // winutil
