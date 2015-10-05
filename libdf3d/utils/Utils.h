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

inline std::vector<std::string> split(const std::string &str, char delim)
{
    std::vector<std::string> result;

    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim))
        result.push_back(item);

    return result;
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

} }
