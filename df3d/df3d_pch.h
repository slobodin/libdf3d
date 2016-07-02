#pragma once

#ifdef _MSC_VER
#pragma warning(disable: 4458)  // Declaration hides class member
#endif

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
#include <unordered_set>
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
#include <memory>
#include <iterator>

#include "df3d_config.h"
#include "libdf3d_dll.h"

#include <df3d/Common.h>

#include <df3d/lib/memory/Allocator.h>
#include <df3d/lib/memory/MallocAllocator.h>
#include <df3d/lib/containers/PodArray.h>
