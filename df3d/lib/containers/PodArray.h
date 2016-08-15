#pragma once

namespace df3d {

class Allocator;

//! Doesn't call ctor or dtor on T. Use only for POD types.
template<typename T>
class DF3D_DLL PodArray final
{
    T *m_data;
    //! Actual elements count.
    size_t m_size;
    //! Allocated elements count.
    size_t m_capacity;
    //! Allocator being used by the array.
    Allocator *m_allocator;

    static void swap(PodArray<T> &a, PodArray<T> &b)
    {
        std::swap(a.m_data, b.m_data);
        std::swap(a.m_size, b.m_size);
        std::swap(a.m_capacity, b.m_capacity);
        std::swap(a.m_allocator, b.m_allocator);
    }

    void reallocate(size_t newCapacity)
    {
        if (newCapacity == m_capacity)
            return;

        // Copy at most newCapacity of data if new capacity less than current size.
        if (newCapacity < m_size)
            m_size = newCapacity;

        // Copy over the data to new location.
        T *newData = nullptr;
        if (newCapacity > 0)
        {
            newData = (T*)m_allocator->alloc(sizeof(T) * newCapacity, alignof(T));
            memcpy(newData, m_data, sizeof(T) * m_size);
        }

        // Get rid of old data.
        m_allocator->dealloc(m_data);

        m_data = newData;
        m_capacity = newCapacity;
    }

    size_t getGrowSize() const
    {
        return static_cast<size_t>(m_capacity * 1.5f + 4.0f);
    }

public:
    PodArray(Allocator *allocator)
        : m_data(nullptr),
        m_size(0),
        m_capacity(0),
        m_allocator(allocator)
    {

    }

    PodArray(Allocator *allocator, size_t count, const T &def = {})
        : PodArray<T>(allocator)
    {
        resize(count, def);
    }

    PodArray(const PodArray<T> &other)
        : PodArray<T>(other.m_allocator)
    {
        resize(other.m_size);
        memcpy(m_data, other.m_data, sizeof(T) * other.m_size);
    }

    PodArray(PodArray<T> &&other)
        : PodArray<T>(other.m_allocator)
    {
        swap(*this, other);
    }

    PodArray& operator= (PodArray<T> other)
    {
        swap(*this, other);
        return *this;
    }

    ~PodArray()
    {
        m_allocator->dealloc(m_data);
    }

    T& operator[] (size_t i)
    {
        DF3D_ASSERT(i < m_size);
        return m_data[i];
    }

    const T& operator[] (size_t i) const
    {
        DF3D_ASSERT(i < m_size);
        return m_data[i];
    }

    void push_back(const T &element)
    {
        DF3D_ASSERT(m_size <= m_capacity);

        if (m_size == m_capacity)
            reallocate(getGrowSize());

        m_data[m_size++] = element;
    }

    void pop_back()
    {
        DF3D_ASSERT(m_size > 0);
        --m_size;
    }

    void resize(size_t count)
    {
        if (m_size == count)
            return;

        if (m_capacity < count)
            reallocate(std::max(count, getGrowSize()));

        m_size = count;
        DF3D_ASSERT(m_size <= m_capacity);
    }

    void resize(size_t count, const T &def)
    {
        resize(count);
        for (auto &el : *this)
            el = def;
    }

    void reserve(size_t count)
    {
        if (count > m_capacity)
            reallocate(count);

        DF3D_ASSERT(m_size <= m_capacity);
    }

    // Doesn't frees memory.
    void clear()
    {
        m_size = 0;
    }

    void shrink_to_fit()
    {
        reallocate(m_size);
    }

    void assign(const T *input, size_t count)
    {
        resize(count);
        memcpy(m_data, input, count * sizeof(T));
    }

    T* data() { return m_data; }
    const T* data() const { return m_data; }

    bool empty() const { return m_size == 0; }
    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }

    const T& back() const
    {
        DF3D_ASSERT(m_size > 0);
        return m_data[m_size - 1];
    }

    T& back()
    {
        DF3D_ASSERT(m_size > 0);
        return m_data[m_size - 1];
    }

    const T& front() const
    {
        DF3D_ASSERT(m_size > 0);
        return m_data[0];
    }

    T& front()
    {
        DF3D_ASSERT(m_size > 0);
        return m_data[0];
    }

    T* begin() { return m_data; }
    const T* begin() const { return m_data; }
    T* end() { return m_data + m_size; }
    const T* end() const { return m_data + m_size; }
};

}
