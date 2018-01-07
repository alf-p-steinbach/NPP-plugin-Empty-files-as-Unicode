#pragma once
#include <cppx/constness.hpp>                   // cppx::const_like_
#include <cppx/class-kind/No_copy.hpp>
#include <npp/Encoding_config.hpp>              // npp::Encoding_config
#include <npp/paths.hpp>                        // npp::path_to_gui_config
#include <stdlib/extension/hopefully_and_fail.hpp>
#include <stdlib/filesystem.hpp>                // std::filesystem::*
#include <stdlib/memory.hpp>                    // std::unique_ptr
#include <stdlib/string.hpp>                    // std::(stoi, string)
#include <wrapped-tinyxml2/all.hpp>             // tinyxml2::*

namespace npp {
    using namespace std::literals;
    using namespace stdlib::ext::hopefully_and_fail;
    using namespace stdlib::ext::type_builders;

    namespace fs = std::filesystem;
    namespace tx2 = tinyxml2;

    using cppx::const_like_;
    using cppx::Constness;
    using cppx::No_copy;
    using cppx::with_constness_;
    using std::stoi;
    using std::string;
    using std::unique_ptr;
    using stdlib::ext::char_path_or_x;

    using Path = fs::path;

    class Config;

    namespace config_impl {

        template< class Type >
        auto non_null( ref_<const string> msg, const ptr_<Type> p )
            -> ptr_<Type>
        {
            hopefully( p != nullptr )
                or fail( "npp::Config::" + msg );
            return p;
        }

        template< Constness::Enum constness >       // m or c, for mutable or const
        class Const_adaptation_
        {
        protected:
            using Self = with_constness_<constness, Config>;

            static auto root( ref_<Self> self )
                -> ptr_<const_like_<Self, tx2::XMLElement>>
            {
                return non_null( "root - no xml root element", self.p_xml_->RootElement() );
            }

            static auto gui_configs( ref_<Self> self )
                -> ptr_<const_like_<Self, tx2::XMLElement>>
            {
                return non_null(
                    "gui_configs - no xml GUIConfigs element",
                    root( self )->FirstChildElement( "GUIConfigs" )
                    );
            }

            static auto config_of_new_doc( ref_<Self> self )
                -> ptr_<const_like_<Self, tx2::XMLElement>>
            {
                auto p_config = gui_configs( self )->FirstChildElement();
                while( p_config != nullptr )
                {
                    const ptr_<const char> p_name = p_config->Attribute( "name" );
                    if( p_name != nullptr and p_name == "NewDocDefaultSettings"s )
                    {
                        break;
                    }
                    p_config = p_config->NextSiblingElement();
                }
                return non_null(
                    "config_of_new_doc - no xml element named NewDocDefaultSettings",
                    p_config
                    );
            }
        };
    }  // namespace config_impl

    class Config
        : public No_copy
        , private config_impl::Const_adaptation_<Constness::m>
        , private config_impl::Const_adaptation_<Constness::c>
    {
        unique_ptr<tx2::XMLDocument>    p_xml_;     // Dynamic in order to be non-const.

        using Consted = config_impl::Const_adaptation_<Constness::c>;  friend Consted;
        using Mutable = config_impl::Const_adaptation_<Constness::m>;  friend Mutable;

        using Consted::config_of_new_doc;
        using Mutable::config_of_new_doc;

    public:
        auto default_encoding() const
            -> Encoding_config
        {
            return Encoding_config{ *config_of_new_doc( *this ) };
        }

        void set_default_encoding( ref_<const Encoding_config> values )
        {
            values.set_as_attributes_in( *config_of_new_doc( *this ) );
        }

        auto to_string() const
            -> string
        {
            tx2::XMLPrinter printer;
            p_xml_->Print( &printer );
            return printer.CStr();
        }

        Config( ref_<const Path> path = path_to_gui_config() )
            : p_xml_{ new tx2::XMLDocument{} }
        {
            hopefully( fs::exists( path ) )
                or fail( "npp::Config::<init> - specified config file doesn't exist" );
            const string ansi_config_path = char_path_or_x( path );
            p_xml_->LoadFile( ansi_config_path.c_str() ) >> Success{}
                or fail( "npp::Config::<init> - tinyxml2::XMLDocument::LoadFile failed" );
            //hopefully( xml_.RootElement()->Name() == "NotepadPlus"s )
            //    or fail( "npp::Config::<init> -  XML root element is not “NotepadPlus”" );
        }
    };

}  // namespace npp
