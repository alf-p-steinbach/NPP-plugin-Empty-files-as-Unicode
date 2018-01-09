#pragma once

#include "menu.hpp"
#include "plugin-name.hpp"

#include <cppx/class-kind/No_copy_or_move.hpp>
#include <cppx/debug/console-output.hpp>        // cppx::debug::*
#include <cppx/debug/CPPX_DBGINFO.hpp>
#include <cppx/stdlib-wrappers/Map_.hpp>        // cppx::Map_
#include <cppx/text/stdstring_util.hpp>         // cppx::wide_from_ascii

#include <npp/Config.hpp>
#include <npp/Npp.hpp>

#include <stdlib/extension/hopefully_and_fail.hpp>
#include <stdlib/extension/type_builders.hpp>
#include <stdlib/extension/Size.hpp>            // stdlib::ext::(n_items_of, array_size, Size, U_size)

#include <stdlib/bitset.hpp>
#include <stdlib/functional.hpp>                // std::invoke
#include <iostream>                             // std::wclog
#include <stdlib/memory.hpp>                    // std::(make_unique, unique_ptr)
#include <stdlib/string.hpp>                    // std::wstring

#include <wrapped-notepad++/plugin-dll-interface.hpp>   // Func_item etc.

namespace plugin{ namespace impl {
    using cppx::Map_;
    using cppx::No_copy_or_move;
    using cppx::wide_from_ascii;
    namespace debug = cppx::debug;

    constexpr auto& dbginfo = cppx::debug::info;    // For CPPX_DBGINFO.

    using stdlib::ext::array_size;
    using stdlib::ext::fail;
    using stdlib::ext::n_items_of;
    using stdlib::ext::U_size;
    using namespace stdlib::ext::type_builders;     // raw_array_, ref_

    using std::bitset;
    using std::exception;
    using std::invoke;
    using std::to_wstring;
    using std::unique_ptr;
    using std::wstring;

    auto const about_text =
//L"Let 𝘜 be UTF-8 with BOM.\n
L"The current default Unicode encoding 𝘜 is ¤.\n\
\n\
Without this plugin Notepad++ will assume that any existing empty file is encoded\
 as Windows ANSI. With this plugin a Unicode encoding is assumed, and used when you\
 add and save some text.\n\
\n\
Technically, here’s how it works. Let 𝘜 be the encoding last saved as default in the\
 general Notepad++ options, or, if that encoding isn’t Unicode, then 𝘜 is UTF-8 with\
 BOM.  Note: Notepad++ only saves the options when you exit the program. Thus, to\
 see any changed default as 𝘜 you’ll have to relaunch the program.\n\
\n\
When a buffer is activated and has not already been checked:\n\
\n\
      if the document is empty and\n\
      its encoding isn’t Unicode, then\n\
\n\
      its encoding is set to 𝘜.\n\
\n\
Author’s mail address: alf.p.steinbach+npp@gmail.com";

    class Plugin
        : public No_copy_or_move        // A singleton
    {
        using Buffer_id = npp::Buffer_id;
        struct Codepage_id{ enum Enum: int {}; };

        using Buffer_codepage_map = Map_<Buffer_id::Enum, Codepage_id::Enum>;

        Npp                         npp_;
        bool                        npp_startup_completed_  = false;
        bool                        is_disabled_            = false;
        Buffer_codepage_map         checked_buffers_        = {};
        npp::File_encoding::Enum    default_encoding_       = npp::File_encoding::utf8_with_bom;

        void best_effort_check( const Buffer_id::Enum buffer_id )
        {
            using Enc = npp::File_encoding::Enum;
            const Enc npp_file_encoding = npp_.file_encoding( buffer_id );

            const auto filepath = npp_.doc_path_for( buffer_id );
            CPPX_DBGINFO(
                to_wstring( buffer_id ) + L" [" + filepath + L"]\n"
                + L"Outer file encoding = " + name_of( npp_file_encoding )
                );

            const bool is_unicode = invoke( [&]() -> bool
            {
                static const bitset<8> unicodes = 0u
                    | (1u << Enc::utf8_with_bom)
                    | (1u << Enc::utf16_be_with_bom)
                    | (1u << Enc::utf16_le_with_bom)
                    | (1u << Enc::utf8)
                    | (1u << Enc::utf16_be)
                    | (1u << Enc::utf16_le);
                return unicodes[npp_file_encoding];
            } );

            //CPPX_DBGINFO( wstring() + L"is_unicode = " + (is_unicode? L"T" : L"F") );
            if( not is_unicode )
            {
                if( buffer_id != npp_.current_buffer_id() )
                {
                    // Can practically only change encoding of current buffer.
                    fail( "Plugin::check - not current, can't change encoding" );
                }

                if( npp_.scintilla_length() == 0 )
                {
                    CPPX_DBGINFO( filepath + L"\nCONVERTING to Unicode" );
                    //npp_.convert_to_utf8_with_bom();
                    npp_.convert_to( default_encoding_ );
                }
            }

            checked_buffers_.emplace( buffer_id, Codepage_id::Enum{} ); // TODO:
        }

        void update_menus()
        {
            using Static_id = menu::Static_cmd_id;
            const auto set_item_check = [&]( const Static_id::Enum i, const bool value )
            {
                npp_.set_menu_item_check( menu::dynamic_cmd_id( i ), value );
            };

            set_item_check( Static_id::set_enabled, not is_disabled_ );
            set_item_check( Static_id::set_disabled, is_disabled_ );
        }

        void set_disabled( const bool new_value )
        {
            if( is_disabled_ != new_value )
            {
                is_disabled_ = new_value;
                update_menus();
            }
        }

    public:
        static constexpr auto name = plugin::name;

        void infobox( ref_<const wstring> text, ref_<const wstring> title = name )
        {
            npp_.infobox( text, title );
        }

        void cmd_about()
        {
            const auto title = wstring() + L"About the “" + name + L"” plugin";
            const ptr_<const wchar_t> p_insert = wcschr( about_text, L'¤' );
            wstring text;
            if( p_insert == nullptr )
            {
                text = about_text;
            }
            else
            {
                text = wstring{ about_text, p_insert };
                text += name_of( default_encoding_ );
                text += p_insert + 1;
            }
            infobox( text, title );
        }

        void cmd_show_doc_info()
        {
            const npp::File_encoding::Enum encoding = npp_.file_encoding();
            infobox(
                L"File: [" + npp_.current_doc_path() + L"]\n" +
                L"File_encoding: " + name_of( encoding )
                );
        }

        void cmd_check_all()
        {
            auto_check_all();
        }

        void cmd_set_enabled()
        {
            set_disabled( false );
        }

        void cmd_set_disabled()
        {
            set_disabled( true );
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
                CPPX_DBGINFO( L"! " + wide_from_ascii( x.what() ) );
            }
        }

        void auto_check( const Buffer_id::Enum buffer_id )
        {
            if( not in( checked_buffers_, buffer_id ) )
            {
                check( buffer_id );
            }
        }

        void auto_check_all()
        {
            for( const auto buffer_id : npp_.buffer_ids() )
            {
                auto_check( buffer_id );
            }
        }

        void on_notification( const npp::Notification_id id, const Buffer_id::Enum buffer_id = {} )
        {
            if( is_disabled_ )
            {
                return;
            }

            switch( id )
            {
                case NPPN_BUFFERACTIVATED:
                {
                    // During startup dummy documents are created and activated, and
                    // changing the encoding of such can create a second window pane.
                    if( npp_startup_completed_ )
                    {
                        auto_check( npp_.current_buffer_id() );
                    }
                    break;
                }
                case NPPN_FILEOPENED:
                {
                    CPPX_DBGINFO( L"NPPN_FILEOPENED" );
                    break;
                }
                case NPPN_FILEBEFORECLOSE:
                {
                    checked_buffers_.erase( buffer_id );
                    break;
                }
                case NPPN_READY:
                {
                    auto_check_all();
                    update_menus();
                    npp_startup_completed_ = true;
                    break;
                }
                case NPPN_SHUTDOWN:
                {
                    CPPX_DBGINFO( L"Closing Notepad++..." );
                    break;
                }
            }
        }

        Plugin( ref_<const NppData> npp_handles )
            : npp_{ npp_handles } 
        {
            debug::init_console();

            try
            {
                const auto encoding_info = npp::Config{}.default_encoding();
                if( encoding_info.is_unicode() )
                {
                    default_encoding_ = encoding_info.main_encoding();
                }
            }
            catch( ... )
            {}
        }
    };

}}  // namespace plugin::impl

using plugin::impl::Plugin;
