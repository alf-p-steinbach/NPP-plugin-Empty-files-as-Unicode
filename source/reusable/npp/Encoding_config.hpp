#pragma once
#include <stdlib/extension/type_builders.hpp>   // stdlib::ext::(ref_, ptr_)
#include <wrapped-notepad++/basic-types.hpp>    // npp::File_encoding::Enum
#include <wrapped-tinyxml2/all.hpp>             // tinyxml2::*

namespace npp{
    using namespace stdlib::ext::type_builders;
    namespace tx2 = tinyxml2;

    class Encoding_config
    {
        int                     codepage_           = -1;       // Don't care.
        File_encoding::Enum     main_encoding_      = File_encoding::utf8_with_bom;
        bool                    open_ansi_as_utf8_  = false;

        static auto int_or_default(
            const int                       default_value,
            const ptr_<const char>          name,
            ref_<const tx2::XMLElement>     elem
            )
            -> int
        {
            int result = default_value;
            elem.QueryIntAttribute( name, &result );
            return result;
        }

        static auto bool_or_default(
            const bool                      default_value,
            const ptr_<const char>          name,
            ref_<const tx2::XMLElement>     elem
            )
            -> bool
        {
            bool result = default_value;
            elem.QueryBoolAttribute( name, &result );
            return result;
        }

    public:
        auto main_encoding() const      -> File_encoding::Enum  { return main_encoding_; }
        auto codepage() const           -> int                  { return codepage_; }
        auto auto_conversion() const    -> bool                 { return open_ansi_as_utf8_; }

        auto is_unicode() const
            -> bool
        {
            return
                codepage_ == 65001      // Just in case Notepad++ uses or will use that.
                or
                (
                    codepage_ == -1     // With other values the main encoding is ignored.
                    and main_encoding_ != File_encoding::ascii
                    and main_encoding_ != File_encoding::ansi
                );
        }

        void set_as_attributes_in( ref_<tx2::XMLElement> elem ) const
        {
            elem.SetAttribute( "codepage", codepage_ );
            elem.SetAttribute( "encoding", +main_encoding_ );
            elem.SetAttribute( "openAnsiAsUTF8", open_ansi_as_utf8_ );
        }

        Encoding_config() = default;

        Encoding_config( const File_encoding::Enum new_main_encoding )
            : codepage_{ -1 }
            , main_encoding_{ new_main_encoding }
            , open_ansi_as_utf8_{ false
                or main_encoding_ == File_encoding::utf8_with_bom
                or main_encoding_ == File_encoding::utf8
                }
        {}

        Encoding_config( ref_<const tx2::XMLElement> elem )
            : codepage_{
                int_or_default( -1, "codepage", elem )
                }
            , main_encoding_{ static_cast<File_encoding::Enum>(
                int_or_default( File_encoding::utf8_with_bom, "encoding", elem )
                ) }
            , open_ansi_as_utf8_{
                bool_or_default( false, "openAnsiAsUTF8", elem )    // Good enough.
                }
        {}
    };

}  // namespace npp
