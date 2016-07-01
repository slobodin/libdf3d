#pragma once

namespace df3d {

template<typename T>
class ConcurrentQueue : public NonCopyable
{
    mutable std::mutex m_lock;
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

    T pop()
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);

        T result = m_queue.front();
        m_queue.pop();

        return result;
    }

    bool empty() const
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);

        return m_queue.empty();
    }

    void clear()
    {
        std::lock_guard<decltype(m_lock)> lock(m_lock);

        m_queue = std::queue<T>();
    }
};

}
