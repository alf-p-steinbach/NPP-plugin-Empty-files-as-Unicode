cmake_minimum_required( VERSION 3.10 )
project( "Notepad++ encoding fix plugin" )

if( ${CMAKE_SIZEOF_VOID_P} EQUAL 4 )
	set( PLUGIN_NAME_SUFFIX "32-bit" )
elseif( ${CMAKE_SIZEOF_VOID_P} EQUAL 8 )
	set( PLUGIN_NAME_SUFFIX "64-bit" )
else()
    message( FATAL_ERROR "The selected toolchain isn't recognized as 32-bit or 64-bit." )
endif()

include_directories(
    source/extern-api
    )

add_library( empty-files-as-unicode.${PLUGIN_NAME_SUFFIX} MODULE
    source/plugin/menu.cpp
    source/plugin/plugin-dll-interface.cpp
    source/plugin/cppx-impl/console-output.cpp
    )
