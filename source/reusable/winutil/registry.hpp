#pragma once
#include <stdlib/extension/type_builders.hpp>       // stdlib::ext::(ref_, ptr_)
#include <stdlib/extension/hopefully_and_fail.hpp>  // stdllib::ext::(hopefully, fail)
#include <wrapped-windows/windows_h.hpp>

#include <string>           // std::wstring

namespace winutil { namespace reg {
    using std::wstring;
    using namespace stdlib::ext::hopefully_and_fail;
    using namespace stdlib::ext::type_builders;

    const HKEY hkcr     = HKEY_CLASSES_ROOT;
    const HKEY hkcu     = HKEY_CURRENT_USER;
    const HKEY hklm     = HKEY_LOCAL_MACHINE;

    // Summary:
    inline auto string_value( const HKEY key, ptr_<const wchar_t> value_name ) -> wstring;
    inline auto string_value( const HKEY key, ref_<const wstring> value_name ) -> wstring;
    inline void close( const HKEY key );
    inline auto open_direct_subkey_for_read( const HKEY key, ptr_<const wchar_t> subkey_name ) -> HKEY;
    inline auto open_direct_subkey_for_read( const HKEY key, ref_<const wstring> subkey_name ) -> HKEY;
    inline auto open_subkey_for_read( const HKEY key, ptr_<const wchar_t> subkey_path ) -> HKEY;
    inline auto open_subkey_for_read( const HKEY key, ref_<const wstring> subkey_path ) -> HKEY;

    inline auto string_value( const HKEY key, ptr_<const wchar_t> value_name )
        -> wstring
    {
        DWORD n_bytes = 0;
        const auto size_call_code = RegQueryValueEx(
            key, value_name, 0, nullptr, nullptr, &n_bytes
            );
        hopefully( size_call_code == ERROR_SUCCESS )
            or fail( "winutil::reg::value - RegQueryValueEx get-size call failed" );
        assert( n_bytes % sizeof( wchar_t ) == 0 );
        if( n_bytes == 0 )
        {
            return L"";
        }
        wstring result( n_bytes, '\0' );
        const auto value_call_code = RegQueryValueEx(
            key, value_name, 0, nullptr, reinterpret_cast<LPBYTE>( &result[0] ), &n_bytes
            );
        hopefully( value_call_code == ERROR_SUCCESS )
            or fail( "winutil::reg::value - RegQueryValueEx get-value call failed" );
        assert( n_bytes % sizeof( wchar_t ) == 0 );
        int n_chars = n_bytes/sizeof(wchar_t);
        if( n_chars > 0 and result[n_chars - 1] == L'\0' )
        {
            --n_chars;
        }
        result.resize( n_chars );
        return result;
    }

    inline auto string_value( const HKEY key, ref_<const wstring> value_name )
        -> wstring
    { return string_value( key, value_name.c_str() ); }

    inline void close( const HKEY key )
    {
        RegCloseKey( key );
    }

    inline auto open_direct_subkey_for_read( const HKEY key, ptr_<const wchar_t> subkey_name )
        -> HKEY
    {
        HKEY result;
        const auto code = RegOpenKeyEx(
            key,
            subkey_name,
            0,          // ulOptions
            KEY_READ,
            &result
            );
        hopefully( code == ERROR_SUCCESS )
            or fail( "winutil::reg::open_for_reading - RegOpenKeyEx failed" );
        return result;
    }

    inline auto open_direct_subkey_for_read( const HKEY key, ref_<const wstring> subkey_name )
        -> HKEY
    { return open_direct_subkey_for_read( key, subkey_name.c_str() ); }

    inline auto open_subkey_for_read( const HKEY key, ref_<const wstring> subkey_path )
        -> HKEY
    {
        const int len = subkey_path.length();
        HKEY result = key;
        int i_start = 0;
        for( ;; )
        {
            const auto j = subkey_path.find( L"\\", i_start );
            const int i_end = (j == wstring::npos? len : static_cast<int>( j ));
            const HKEY new_result = open_direct_subkey_for_read(
                result, subkey_path.substr( i_start, i_end - i_start ).c_str()
                );
            if( result != key )
            {
                close( result );
            }
            result = new_result;
            if( i_end == len )
            {
                break;
            }
            i_start = i_end + 1;
        }
        return result;
    }

    inline auto open_subkey_for_read( const HKEY key, ptr_<const wchar_t> subkey_path )
        -> HKEY
    { return open_subkey_for_read( key, wstring{ subkey_path } ); }

}}  //  namespace winutil::reg
