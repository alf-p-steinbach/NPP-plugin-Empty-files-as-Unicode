#pragma once
#include <notepad++/original/Scintilla.h>
#include <windows/windows_h.hpp>

namespace scintilla {

    inline auto codepage( const HWND handle )
        -> int
    { return static_cast<int>( ::SendMessage( handle, SCI_GETCODEPAGE, 0, 0 ) ); }

    inline auto length( const HWND handle )
        -> int
    { return static_cast<int>( ::SendMessage( handle, SCI_GETLENGTH, 0, 0 ) ); }

}  // namespace scintilla
