#pragma once

#include <winutil/hresult-success-checking.hpp>
#include <windows/objbase_h.hpp>        // CoInitialize etc.

namespace winutil{ namespace lib_envelope{
    using namespace stdlib::ext::hopefully_and_fail;

    struct Com
    {
        ~Com()
        {
            ::CoUninitialize();
        }

        Com()
        {
            ::CoInitialize( 0 ) >> Success{}
                or fail( "winutil::lib_envelope::Com::<init> - CoInitialize failed" );
        }
    };

}}  // namespace winapi::lib_envelope
