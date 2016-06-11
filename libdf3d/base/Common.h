#pragma once

// Common includes.
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <queue>
#include <list>
#include <set>
#include <stdexcept>
#include <memory>
#include <chrono>
#include <csignal>

#define GLM_FORCE_RADIANS
//#define GLM_MESSAGES
//#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <json/json.h>

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::weak_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;

// Common macros.
#if defined(DF3D_WINDOWS)
#define DEBUG_BREAK() __debugbreak()
#elif defined(DF3D_IOS)
#define DEBUG_BREAK() std::raise(SIGTRAP)
#else
#define DEBUG_BREAK() ((void) 0)
#endif

#ifndef _MSC_VER
#define LIKELY(expr) __builtin_expect(!!(expr), 1)
#define UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#define LIKELY(expr) expr
#define UNLIKELY(expr) expr
#endif

#if defined(max)
#undef max
#endif

#if defined(min)
#undef min
#endif

#ifdef _DEBUG

#define DF3D_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            DFLOG_CRITICAL("Assertion failed: %s. File: %s, line: %d", #cond, __FILE__, __LINE__); \
            DEBUG_BREAK(); \
        } \
    } while (0)

#define DF3D_ASSERT_MESS(cond, msg) \
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
#define DF3D_ASSERT_MESS(cond, msg) ((void) 0)

#endif

namespace df3d {

enum class ResourceLoadingMode
{
    IMMEDIATE,
    ASYNC
};

enum class PixelFormat
{
    INVALID,
    RGB,
    RGBA,
    DEPTH
};

using ResourceGUID = std::string;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

inline float IntervalBetween(const TimePoint &t1, const TimePoint &t2)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t2).count() / 1000.f;
}

// seconds
inline float IntervalBetweenNowAnd(const TimePoint &timepoint)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timepoint).count() / 1000.0f;
}

static const int DEFAULT_WINDOW_WIDTH = 640;
static const int DEFAULT_WINDOW_HEIGHT = 480;

}

// Include some useful df3d stuff.

#include <libdf3d/base/Log.h>
#include <libdf3d/utils/NonCopyable.h>
