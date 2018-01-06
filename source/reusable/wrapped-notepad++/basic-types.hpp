#pragma once

namespace npp {

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
        static constexpr Enum n_values = _;
    };

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
