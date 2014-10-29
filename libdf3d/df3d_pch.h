#pragma once

#pragma warning(disable : 4458)

#include <SDL.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstdint>
#include <cfloat>
#include <climits>

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <stack>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

#include "df3d_config.h"
#include "libdf3d_dll.h"

#include <boost/smart_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/function.hpp>
#include <boost/functional/hash.hpp>

using boost::shared_ptr;
using boost::make_shared;
using boost::scoped_ptr;
using boost::weak_ptr;
using boost::intrusive_ptr;
using boost::dynamic_pointer_cast;
using boost::static_pointer_cast;

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <json/json.h>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include <base/Log.h>
#include <base/MacroDefs.h>
#include <base/Common.h>
