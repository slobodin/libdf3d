#pragma once

namespace df3d {

#define DF3D_MAKE_SHORT_HANDLE(name) struct name { \
    int16_t id; \
    name(int16_t id = -1) : id(id) { } \
    bool valid() const { return id != -1; } \
    void invalidate() { id = -1; } \
    int16_t getId() const { return id; } \
    bool operator== (const name &other) const { return other.id == id; } \
    bool operator!= (const name &other) const { return other.id != id; } \
    bool operator< (const name &other) const { return id < other.id; } };

#define DF3D_MAKE_HANDLE(name) struct name { \
    int32_t id; \
    name(int32_t id = -1) : id(id) { } \
    bool valid() const { return id != -1; } \
    void invalidate() { id = -1; } \
    int32_t getId() const { return id; } \
    bool operator== (const name &other) const { return other.id == id; } \
    bool operator!= (const name &other) const { return other.id != id; } \
    bool operator<(const name &other) const { return id < other.id; } };

template<typename T>
class DF3D_DLL HandleBag
{
    int32_t m_maxSize;

    T m_next;

    std::list<T> m_removed;
    std::list<T> m_available;

    T getNextId()
    {
        if (!m_available.empty())
        {
            auto res = m_available.front();
            m_available.pop_front();
            return res;
        }

        T res = m_next;

        ++m_next.id;

        return res;
    }

public:
    HandleBag(int32_t maxSize)
        : m_maxSize(maxSize)
    {
        m_next.id = 0;
    }

    ~HandleBag()
    {

    }

    T getNew()
    {
        if (m_next.id >= m_maxSize)
            return T(-1);

        return getNextId();
    }

    void release(T d)
    {
        m_removed.push_back(d);
    }

    void cleanup()
    {
        m_available.insert(m_available.end(), m_removed.begin(), m_removed.end());
        m_removed.clear();
    }
};

template<typename T, size_t SIZE>
class DF3D_DLL StaticHandleBag
{
    T m_removed[SIZE];
    T m_available[SIZE];

    T m_next;
    size_t m_removedSize = 0;
    size_t m_availableSize = 0;

    T getNextId()
    {
        if (m_availableSize > 0)
            return m_available[--m_availableSize];

        T res = m_next;

        ++m_next.id;

        return res;
    }

public:
    StaticHandleBag()
    {
        m_next.id = 0;
    }

    ~StaticHandleBag() = default;

    T getNew()
    {
        if (m_next.id >= SIZE)
            return T(-1);

        return getNextId();
    }

    void release(T d)
    {
        m_removed[m_removedSize++] = d;
    }

    void cleanup()
    {
        if (m_removedSize != 0)
        {
            std::copy(m_removed, m_removed + m_removedSize, m_available + m_availableSize);
            m_availableSize += m_removedSize;
            m_removedSize = 0;
        }
    }
};

}
