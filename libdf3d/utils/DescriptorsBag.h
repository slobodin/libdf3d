#pragma once

namespace df3d { namespace utils {

template<typename T>
class DF3D_DLL DescriptorsBag
{
    int64_t m_maxSize;

    T m_next = 0;

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

        return m_next++;
    }

public:
    DescriptorsBag(int64_t maxSize)
        : m_maxSize(maxSize)
    {

    }

    ~DescriptorsBag() = default;

    T getNew()
    {
        if (m_next >= m_maxSize)
            return{};

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

} }
