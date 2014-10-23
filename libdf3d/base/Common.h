#pragma once

#include <chrono>

typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

namespace df3d {

DF3D_DLL std::string glmVecToString(const glm::vec3 &v);

inline void SafeNormalize(glm::vec3 &v)
{
    if (glm::fastLength(v) > 0.00001f)
        v = glm::normalize(v);
}

inline float IntervalBetween(const TimePoint &t1, const TimePoint &t2)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t2).count() / 1000.f;
}

// seconds
inline float IntervalBetweenNowAnd(const TimePoint &timepoint)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timepoint).count() / 1000.0f;
}

}