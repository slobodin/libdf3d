#pragma once

#include <chrono>

namespace Rocket { namespace Core { class Element; class ElementDocument; } }

typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;
typedef intrusive_ptr<Rocket::Core::ElementDocument> RocketDocument;
typedef intrusive_ptr<Rocket::Core::Element> RocketElement;

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