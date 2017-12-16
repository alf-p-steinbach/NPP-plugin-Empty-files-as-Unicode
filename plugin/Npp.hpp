#pragma once
#include <notepad++/command-ids.hpp>
#include <notepad++/window-messages.hpp>
#include <notepad++/basic-types.hpp>
#include <source/plugin-name.hpp>
#include <stdlib/extension/hopefully_and_fail.hpp>      // stdlib::(fail, hopefully)
#include <stdlib/extension/type_builders.hpp>           // stdlib::(raw_array_of_, ref_)

#include <stddef.h>     // size_t
#include <string>       // std::wstring
#include <vector>       // std::vector

namespace npp_impl {
    using std::size;
    using std::vector;
    using std::wstring;
    using stdlib::fail;
    using stdlib::hopefully;
    using namespace stdlib::type_builders;      // raw_array_of_, ref_

    class Npp
    {
        HWND    handle_;

    public:
        using Buffer_id         = npp::Buffer_id;
        using File_encoding          = npp::File_encoding;
        using View_id           = npp::View_id;
        using X_view_id         = npp::Extended_view_id;

        auto handle() const -> HWND { return handle_; }

        auto n_open_files_in( const X_view_id::Enum view_id ) const
            -> int
        {
            const LRESULT msg_result = ::SendMessage(
                handle(), NPPM_GETNBOPENFILES, 0, LPARAM( view_id )
                );
            return static_cast<int>( msg_result );
        }

        auto n_open_files_in( const View_id::Enum view_id ) const
            -> int
        { return n_open_files_in( X_view_id::from( view_id ) ); }

        auto buffer_ids() const
            -> vector<Buffer_id::Enum>
        {
            vector<Buffer_id::Enum> result;
            for( const auto view : {View_id::main, View_id::secondary} )
            {
                for( int i = 0, n = n_open_files_in( view ); i < n; ++i )
                {
                    auto const buffer_id = static_cast<Buffer_id::Enum>( ::SendMessage(
                        handle(), NPPM_GETBUFFERIDFROMPOS, i, view
                        ) );
                    result.push_back( buffer_id );
                }
            }
            return result;
        }

        auto current_buffer_id() const
            -> Buffer_id::Enum
        {
            const LRESULT result = ::SendMessage( handle(), NPPM_GETCURRENTBUFFERID, 0, 0 );
            return static_cast<Buffer_id::Enum>( result );
        }

        auto file_encoding( const Buffer_id::Enum id ) const
            -> File_encoding::Enum
        {
            const LRESULT result = ::SendMessage(
                handle(), NPPM_GETBUFFERENCODING, static_cast<WPARAM>( id ), 0
                );
            hopefully( result != -1 )
                or fail( "Npp::file_encoding - NPPM_GETBUFFERENCODING msg failed." );
            return static_cast<File_encoding::Enum>( result );
        }

        auto file_encoding() const
            -> File_encoding::Enum
        { return file_encoding( current_buffer_id() ); }

        auto current_doc_path() const
            -> wstring
        {
            wstring result( MAX_PATH, L'\0' );
            const auto path_address = reinterpret_cast<LPARAM>( &result[0] );
            ::SendMessage( handle(), NPPM_GETFULLCURRENTPATH, result.size(), path_address );
            result.resize( wcslen( &result[0] ) );
            return result;
        }

        auto doc_path_for( const Buffer_id::Enum id ) const
            -> wstring
        {
            const auto n_chars = ::SendMessage( handle(), NPPM_GETFULLPATHFROMBUFFERID, WPARAM( id ), 0 );
            if( n_chars == 0 )
            {
                return L"";
            }
            wstring result( n_chars + 1, L'\0' );
            const auto path_address = reinterpret_cast<LPARAM>( &result[0] );
            ::SendMessage( handle(), NPPM_GETFULLPATHFROMBUFFERID, WPARAM( id ), path_address );
            result.resize( n_chars );
            return result;
        }

        void convert_to_utf8()
        {
            ::SendMessage( handle(), NPPM_MENUCOMMAND, 0, IDM_FORMAT_CONV2_AS_UTF_8 );
        }

        void convert_to_utf8_with_bom()
        {
            ::SendMessage( handle(), NPPM_MENUCOMMAND, 0, IDM_FORMAT_CONV2_UTF_8 );
        }

        void infobox(
            ref_<const wstring>     text,
            ref_<const wstring>     title = plugin::name
            ) const
        { ::MessageBox( handle(), text.c_str(), title.c_str(), MB_ICONINFORMATION ); }

        Npp( const HWND handle ): handle_{ handle } {}
    };
}  // namespace npp_impl

using npp_impl::Npp;
