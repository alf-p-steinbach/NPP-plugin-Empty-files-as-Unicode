#pragma once

#include <cppx/class_kind/No_copy_or_move.hpp>
#include <notepad++/plugin-dll-interface.hpp>   // Func_item etc.
#include <source/ALF_DBGINFO.hpp>
#include <source/Npp.hpp>
//#include <stdlib/iostream.hpp>                  // std::wclog //! Ungood with reset of handles.
#include <stdlib/extension/hopefully_and_fail.hpp>
#include <stdlib/extension/type_builders.hpp>
#include <stdlib/extension/Size.hpp>            // stdlib::(n_items_of, array_size, Size, U_size)

#include <functional>       // std::invoke
#include <iostream>         // std::wclog
#include <memory>           // std::(make_unique, unique_ptr)
#include <string>           // std::wstring
#include <unordered_map>    // std::unordered_map
#include <unordered_set>    // std::unordered_set

namespace cppx{
    using std::unordered_map;
    using std::unordered_set;
    using namespace stdlib::type_builders;

    inline namespace set_operations {

        template< class Key, class Value >      using Map = unordered_map<Key, Value>;
        template< class Value >                 using Set = unordered_set<Value>;

        template< class Key, class Value, class Arg >
        auto in( ref_<const Map<Key, Value>> map, ref_<const Arg> v )
            -> bool
        { return map.count( v ) > 0; }

        template< class Value, class Arg >
        auto in( ref_<const Set<Value>> set, ref_<const Arg> v )
            -> bool
        { return set.count( v ) > 0; }

    } // namespace set_operations
}  // namespace cppx

namespace plugin_impl {
    using cppx::No_copy_or_move;
    using cppx::in;
    using namespace cppx::set_operations;

    using stdlib::array_size;
    using stdlib::fail;
    using stdlib::n_items_of;
    using stdlib::U_size;
    using namespace stdlib::type_builders;  // raw_array_of_, ref_

    using std::exception;
    using std::invoke;
    using std::to_wstring;
    using std::unique_ptr;
    using std::wstring;

    inline void dbginfo( ref_<const wstring> s )
    {
        using namespace std;
        const auto indent = wstring( 4, L' ' );
        wstring formatted = indent;
        for( const wchar_t ch : s )
        {
            formatted += ch;
            if( ch == L'\n' ) { formatted += indent; }
        }
        formatted[0] = L'*';        // L'•';
        wclog << formatted << endl;
    }

    inline auto wide_from_ascii( char const s[] )
        -> wstring
    { return wstring( s, s + strlen( s ) ); }

    auto const about_text = //unflowed( flowed_about_text );
//L"Let 𝘜 be the encoding last saved as default, or, if that encoding isn’t Unicode,\
// let 𝘜 be UTF-8 with BOM.\n\ 
L"Let 𝘜 be UTF-8 with BOM.\n\
\n\
When a buffer is activated and has not already been checked:\n\
\n\
      if the document is empty and its encoding isn’t Unicode, then\n\
      its encoding is set to 𝘜.\n\
\n\
Ideally the “when a buffer…” should have been “when file a is opened or\
 a new document is created”, but\
 apparently Notepad++ does not inform a plugin of its creation of new\
 documents. Also, ideally the forced encoding should have been the one\
 currently selected as default in Notepad++, but apparently Notepad++ does not\
 make the dynamic configuration info available to a plugin.\n\
\n\
Author’s mail address: alf.p.steinbach+npp@gmail.com";

    class Plugin
        : public No_copy_or_move        // A singleton
    {
        using Buffer_id = npp::Buffer_id;
        struct Codepage_id{ enum Enum: int {}; };

        using Buffer_codepage_map = Map<Buffer_id::Enum, Codepage_id::Enum>;

        Npp                     npp_;
        bool                    npp_startup_completed_  = false;
        Buffer_codepage_map     checked_buffers_        = {};

        static auto name_of( const npp::File_encoding::Enum e )
            -> ptr_<const wchar_t>
        {
            static const raw_array_<ptr_<const wchar_t>> names =
            {
                L"8 bit (0)",                       // uni_8bit      
                L"UTF8 with BOM (1)",               // uni_UTF8      
                L"UTF16 BE with BOM (2)",           // uni_16BE      
                L"UTF16 LE with BOM (3)",           // uni_16LE      
                L"cookie (4)",                      // uni_cookie    
                L"7 bit (5)",                       // uni_7Bit      
                L"UTF16 BE (6)",                    // uni_16BE_noBOM
                L"UTF6LE (7)"                       // uni_16LE_noBOM
            };

            return (0?0
                : e < 0?                                L"unknown"
                : e >= npp::File_encoding::n_values?    L"unnamed"
                : /* default */                         names[e]
                );
        }

        void best_effort_check( const Buffer_id::Enum buffer_id )
        {
            using Enc = npp::File_encoding::Enum;
            const Enc npp_file_encoding = npp_.file_encoding( buffer_id );

            const auto filepath = npp_.doc_path_for( buffer_id );
            ALF_DBGINFO(
                to_wstring( buffer_id ) + L" [" + filepath + L"]\n"
                + L"Outer file encoding = " + name_of( npp_file_encoding )
                );

            const bool is_unicode = invoke( [&]() -> bool
            {
                if( npp_file_encoding == Enc::uni_cookie )
                {
                    if( buffer_id != npp_.current_buffer_id() )
                    {
                        // Can practically only check the current buffer.
                        fail( "Plugin::check - not current, can't get scintilla  codepage" );
                    }

                    // TODO: Check Scintilla codepage
                    return false;
                }
                else
                {
                    // uni_7Bit is presumably pure ASCII.
                    // uni_8Bit is e.g. Windows ANSI.
                    return not(
                        npp_file_encoding == Enc::uni_7Bit or
                        npp_file_encoding == Enc::uni_8bit
                        );
                }
            } );

            //ALF_DBGINFO( wstring() + L"is_unicode = " + (is_unicode? L"T" : L"F") );
            if( not is_unicode )
            {
                if( buffer_id != npp_.current_buffer_id() )
                {
                    // Can practically only change encoding of current buffer.
                    fail( "Plugin::check - not current, can't change encoding" );
                }
                ALF_DBGINFO( filepath + L"\nCONVERTING to UTF-8 with BOM" );
                npp_.convert_to_utf8_with_bom();
            }

            checked_buffers_.emplace( buffer_id, Codepage_id::Enum{} ); // TODO:
        }

    public:
        static constexpr auto name = plugin::name;

        void cmd_about()
        {
            const auto title = wstring() + L"About the “" + name + L"” plugin";
            npp_.infobox( about_text, title );
        }

        void cmd_show_doc_info()
        {
            const npp::File_encoding::Enum encoding = npp_.file_encoding();
            npp_.infobox(
                L"File: [" + npp_.current_doc_path() + L"]\n" +
                L"File_encoding: " + name_of( encoding )
                );
        }

        void check( const Buffer_id::Enum buffer_id )
        {
            try
            {
                best_effort_check( buffer_id );
            }
            catch( ref_<const exception> x )
            {
                (void) x;       // Unused in release build.
                ALF_DBGINFO( L"! " + wide_from_ascii( x.what() ) );
            }
        }

        void auto_check( const Buffer_id::Enum buffer_id )
        {
            if( not in( checked_buffers_, buffer_id ) )
            {
                check( buffer_id );
            }
        }

        void on_notification( const npp::Notification_id id, const Buffer_id::Enum buffer_id = {} )
        {
            switch( id )
	        {
	            case NPPN_BUFFERACTIVATED:
                {
                    // During startup dummy documents are created and activated, but
                    // changing encoding of such can create a second window pane.
                    if( npp_startup_completed_ )
                    {
                        auto_check( npp_.current_buffer_id() );
                    }
		            break;
                }
	            case NPPN_FILEOPENED:
                {
		            break;
                }
                case NPPN_FILEBEFORECLOSE:
                {
                    checked_buffers_.erase( buffer_id );
                    break;
                }
                case NPPN_READY:
                {
                    for( const auto buffer_id : npp_.buffer_ids() )
                    {
                        auto_check( buffer_id );
                    }
                    npp_startup_completed_ = true;
		            break;
                }
                case NPPN_SHUTDOWN:
                {
                    ALF_DBGINFO( L"Closing Notepad++..." );
                    break;
                }
	        }
        }

        Plugin( const HWND npp_handle ): npp_{ npp_handle } 
        {
            if( GetStdHandle( STD_ERROR_HANDLE ) == 0 )
            {
                AttachConsole( ATTACH_PARENT_PROCESS );
                freopen( "con", "w", stderr );
                fprintf( stderr, "\n" );  fflush( stderr );
            }
        }
    };

}  // namespace plugin_impl

using plugin_impl::Plugin;
