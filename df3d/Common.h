#pragma once

// Common includes.
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstdint>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>

#define GLM_FORCE_RADIANS
//#define GLM_MESSAGES
//#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/epsilon.hpp>

#include <json/json/json.h>

#include <df3d/lib/Log.h>
#include <df3d/lib/Id.h>
#include <df3d/lib/NonCopyable.h>
#include <df3d/lib/assert/Assert.h>
#include <df3d/lib/containers/PodArray.h>
#include <df3d/lib/memory/Allocator.h>

using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::weak_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;

#if defined(max)
#undef max
#endif

#if defined(min)
#undef min
#endif

namespace df3d {

enum class SeekDir
{
    BEGIN,
    END,
    CURRENT
};

static const int DEFAULT_WINDOW_WIDTH = 640;
static const int DEFAULT_WINDOW_HEIGHT = 480;

}

namespace Json { class Value; }
