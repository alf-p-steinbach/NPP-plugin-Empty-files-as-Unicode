#pragma once
#include <stdlib/extension/type_builders.hpp>

namespace npp {
    using namespace stdlib::ext::type_builders;     // ptr_, raw_array_

    struct Buffer_id
    {
        enum Enum: int {};
    };

    struct File_encoding         // "UniMode"
    {
        // See e.g. NPPM_GETBUFFERENCODING
        enum Enum: int
        {
            ansi                = 0,           // Uni8bit
            utf8_with_bom       = 1,           // UniUTF8
            utf16_be_with_bom   = 2,           // Uni16BE            
            utf16_le_with_bom   = 3,           // Uni16LE            
            utf8                = 4,           // UniCookie
            ascii               = 5,           // Uni7bit            
            utf16_be            = 6,           // Uni16BE_noBOM      
            utf16_le            = 7,           // Uni16LE_noBOM      
            _
        };
        static auto n_values() -> int { return _; }
    };

    inline auto name_of( const npp::File_encoding::Enum e )
        -> ptr_<const wchar_t>
    {
        static const raw_array_<ptr_<const wchar_t>> names =
        {
            L"Windows ANSI",                    // ansi
            L"UTF8 with BOM",                   // utf8_with_bom
            L"UTF16 BE with BOM",               // utf16_be_with_bom
            L"UTF16 LE with BOM",               // utf16_le_with_bom
            L"UTF-8",                           // utf8
            L"ASCII",                           // ascii
            L"UTF16 BE",                        // utf16_be
            L"UTF6 LE"                          // utf16_le
        };

        return (0?0
            : e < 0?                                L"<unknown encoding value>"
            : e >= npp::File_encoding::n_values()?  L"<unnamed encoding>"
            : /* default */                         names[e]
            );
    }

    struct View_id
    {
        // MAIN_VIEW, SUB_VIEW
        enum Enum: int { main, secondary };
    };

    // For NPPM_GETNBOPENFILES, not same values as above.
    struct Extended_view_id
    {
        // ALL_OPEN_FILES, PRIMARY_VIEW, SECONDARY_VIEW
        enum Enum: int { all, main, secondary };

        static auto from( const View_id::Enum id )
            -> Enum
        { return static_cast<Enum>( id + 1 ); }
    };

}  // namespace npp
