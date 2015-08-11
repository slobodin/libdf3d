#pragma once

#include <sstream>

namespace df3d { namespace utils {

inline size_t RandRange(const size_t a, const size_t b) { return a + (rand() % (b - a + 1)); }
inline int RandRange(const int a, const int b) { return a + (rand() % (b - a + 1)); }
inline float RandRange(const float a, const float b) { return (rand() / (static_cast<float>(RAND_MAX) + 1.0f)) * (b - a) + a; }

inline void skip_line(std::istream &is)
{
    std::string temp;
    std::getline(is, temp);
}

template<typename T>
struct shared_ptr_less
{
    bool operator()(const shared_ptr<T> &a, const shared_ptr<T> &b) const
    {
        return a.get() < b.get();
    }
};

template<typename C, typename T>
inline bool contains(const C &container, const T &val)
{
    return std::find(std::begin(container), std::end(container), val) != std::end(container);
}

template<typename C, typename T>
inline bool contains_key(const C &container, const T &key)
{
    return container.find(key) != container.end();
}

template<typename T>
inline std::string to_string(const T &v)
{
    std::ostringstream os;
    os << v;
    return os.str();
}

template<typename T>
inline T from_string(const std::string &s)
{
    T res;
    std::istringstream is(s);
    is >> res;
    return res;
}

inline bool starts_with(const std::string &s, const std::string &with)
{
    return s.compare(0, with.size(), with) == 0;
}

template<typename T>
inline T clamp(const T &val, const T &min, const T &max)
{
    return std::min(std::max(val, min), max);
}

namespace math {

extern const DF3D_DLL glm::vec4 XAxis;
extern const DF3D_DLL glm::vec4 YAxis;
extern const DF3D_DLL glm::vec4 ZAxis;

// Creates rotation which rotates from v1 to v2.
DF3D_DLL glm::quat fromToRotation(const glm::vec3 &v1, const glm::vec3 &v2);
DF3D_DLL glm::vec2 toPolar(const glm::vec2 &cartesian);
DF3D_DLL float signedDistanceToPlane(const glm::vec4 &plane, const glm::vec3 &point);
DF3D_DLL glm::vec3 safeNormalize(const glm::vec3 &v);

struct DF3D_DLL spherical
{
    float r;
    float yaw;
    float pitch;

    spherical(float r, float yaw, float pitch);
    spherical(const glm::vec3 &v);
    ~spherical();

    glm::vec3 toCartesian();
};

}

} }
