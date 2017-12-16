#pragma once
#include <stdlib.h>         // EXIT_...

namespace cppx{ namespace process{

    namespace impl {
        #ifdef _WIN32
            namespace winapi {
                const int ERROR_INVALID_FUNCTION    = 1;                // Error code
                const int E_FAIL                    = 0x80004005;       // HRESULT
            }  // namespace winapi

            static_assert( EXIT_FAILURE == winapi::ERROR_INVALID_FUNCTION, "!" );
            const int general_failure_code = winapi::E_FAIL;
        #else
            const int general_failure_code = EXIT_FAILURE;
        #endif
    }  // namespace impl

    struct Exit_code
    {
        enum Enum: int { success, failure = impl::general_failure_code };
    };

    static_assert( Exit_code::success == EXIT_SUCCESS, "!" );
    static_assert( Exit_code::failure != 0, "!" );

}}  // namespace cppx::process

namespace cppx{ namespace namespaces{
    namespace process = cppx::process;
}}  // namespace cppx::namespaces
