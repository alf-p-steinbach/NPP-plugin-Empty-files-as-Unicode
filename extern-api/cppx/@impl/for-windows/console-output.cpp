#include <cppx/debug/console-output.hpp>

#ifndef _WIN32
#   error This file is only meaningful for Microsoft Windows.
#endif

#include <windows/windows_h.hpp>

namespace cppx{ namespace debug{

    namespace {
        void init_console( const bool ensure_new_line )
        {
            using impl::has_console;
            has_console() = (GetStdHandle( STD_ERROR_HANDLE ) != 0);
#ifndef NDEBUG
            if( not has_console() )
            {
                has_console() =
                    AttachConsole( ATTACH_PARENT_PROCESS )
                    or AllocConsole();
                if( has_console() )
                {
                    freopen( "con", "w", stderr );
                }
            }
            if( has_console() and ensure_new_line )
            {
                fprintf( stderr, "\n" );  fflush( stderr );
            }
#endif
        }
    }  // namespace <anon>

    void init_console() { init_console( true ); }
    void init_console_no_new_line() { init_console( false ); }

}}  // namespace cppx::debug
