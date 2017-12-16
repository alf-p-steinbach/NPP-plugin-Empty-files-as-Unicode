#pragma once
#include <stdlib/extension/type_builders.hpp>
#include <stdlib/string.hpp>

#include <iostream>         // wclog

namespace cppx{ namespace debug{
    namespace stdlibx = stdlib;
    using namespace stdlibx::type_builders;

    using std::wclog;
    using std::wstring;

    extern void init_console();
    extern void init_console_no_new_line();

    namespace impl {
        inline auto has_console()
            -> bool&
        {
            static bool value;
            return value;
        }
    }  // namespace impl

    inline auto has_console()
        -> bool
    { return impl::has_console(); }

    inline void info( ref_<const wstring> s )
    {
        if( not has_console() or s.empty() ) { return; }

        using namespace std;
        static const auto indent = wstring( 4, L' ' );
        wstring formatted = indent;
        for( const wchar_t ch : s )
        {
            formatted += ch;
            if( ch == L'\n' ) { formatted += indent; }
        }
        formatted[0] = L'•';
        wclog << formatted << endl;
    }

}}  // namespace cppx::debug
