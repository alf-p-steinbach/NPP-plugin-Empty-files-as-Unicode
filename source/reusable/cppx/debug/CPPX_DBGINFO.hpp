// intentionally no #pragma once

#undef CPPX_DBGINFO
#ifdef NDEBUG
#   define CPPX_DBGINFO( s )        []{}()
#else
#   define CPPX_DBGINFO( ... )      dbginfo( __VA_ARGS__ )
#endif
