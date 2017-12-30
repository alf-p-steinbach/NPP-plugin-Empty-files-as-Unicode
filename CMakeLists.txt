cmake_minimum_required( VERSION 3.10 )
project( "Notepad++ encoding fix plugin" )

if( NOT ${CMAKE_SIZEOF_VOID_P} EQUAL 4 )
    message( FATAL_ERROR "!Choose a toolchain that generates a 32-bit DLL (the usual Notepad++ is 32-bit)." )
endif()

include_directories(
    extern-api
    )

add_library( empty-files-as-unicode MODULE
    plugin/menu.cpp
    plugin/plugin-dll-interface.cpp
    plugin/cppx-impl/console-output.cpp
    )
