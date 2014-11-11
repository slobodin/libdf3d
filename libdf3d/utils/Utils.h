#pragma once

namespace df3d { namespace utils {

inline size_t RandRange(const size_t a, const size_t b) { return a + (rand() % (b - a + 1)); }
inline int RandRange(const int a, const int b) { return a + (rand() % (b - a + 1)); }
inline float RandRange(const float a, const float b) { return (rand() / (static_cast<float>(RAND_MAX) + 1.0f)) * (b - a) + a; }

inline void skip_line(std::istream &is)
{
    std::string temp;
    std::getline(is, temp);
}

// gcc doesn't support std::to_string
template<typename T>
inline std::string to_string(T val)
{
    std::stringstream ss;
    ss << val;
    return ss.str();
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

} }