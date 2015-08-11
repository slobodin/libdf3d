#pragma once

namespace df3d { namespace utils { 

template<typename T>
class ConcurrentQueue : public NonCopyable
{
    std::mutex m_lock;
    std::queue<T> m_queue;

public:
    void push(const T &item)
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);

        m_queue.push(item);
    }

    void push(T &&item)
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);

        m_queue.push(std::forward<T>(item));
    }

    bool tryPop(T &dest)
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);

        if (m_queue.empty())
            return false;

        dest = m_queue.front();
        m_queue.pop();

        return true;
    }

    bool empty() const
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);

        return m_queue.empty();
    }
};

} } 
