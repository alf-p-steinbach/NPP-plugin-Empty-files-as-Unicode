#pragma once

#ifdef NDEBUG
#   define ALF_DBGINFO( s )     []{}()
#else
#   define ALF_DBGINFO( ... )   dbginfo( __VA_ARGS__ )
#endif
