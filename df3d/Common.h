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

// Common macroses.
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

enum class SeekDir
{
    BEGIN,
    END,
    CURRENT
};

using ResourceGUID = std::string;

static const int DEFAULT_WINDOW_WIDTH = 640;
static const int DEFAULT_WINDOW_HEIGHT = 480;

}

// Include some useful df3d stuff.

#include <df3d/lib/Log.h>
#include <df3d/lib/NonCopyable.h>
#include <df3d/lib/assert/Assert.h>
