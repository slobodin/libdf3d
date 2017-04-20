#pragma once

#include <csignal>

#include <df3d/lib/Log.h>

namespace df3d {

#if defined(DF3D_WINDOWS)
#define DEBUG_BREAK() __debugbreak()
#elif defined(DF3D_IOS)
#define DEBUG_BREAK() std::raise(SIGTRAP)
#else
#define DEBUG_BREAK() ((void) 0)
#endif

#ifdef _DEBUG
#define DF3D_ASSERTIONS_ENABLED
#endif

#ifdef DF3D_ASSERTIONS_ENABLED

#define DF3D_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            DFLOG_CRITICAL("Assertion failed: %s. File: %s, line: %d", #cond, __FILE__, __LINE__); \
            DEBUG_BREAK(); \
        } \
    } while (0)

#define DF3D_ASSERT_MESS(cond, msg, ...) \
    do { \
        if (!(cond)) { \
            DFLOG_CRITICAL("Assertion failed: %s. File: %s, line: %d", msg, __FILE__, __LINE__); \
            DEBUG_BREAK();\
        } \
    } while (0)

#define DF3D_VERIFY(cond) DF3D_ASSERT(cond)

#else

#define DF3D_VERIFY(cond) do { (void)(cond); } while(0)
#define DF3D_ASSERT(cond) ((void) 0)
#define DF3D_ASSERT_MESS(cond, msg, ...) ((void) 0)

#endif

}
