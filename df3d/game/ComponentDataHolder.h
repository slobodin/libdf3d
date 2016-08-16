#pragma once

#include <df3d/lib/Handles.h>

namespace df3d {

template<typename T>
class DF3D_DLL ComponentDataHolder : NonCopyable
{
public:
    using DestructionCallback = std::function<void(const T&)>;

private:
    const size_t InvalidComponentInstance = std::numeric_limits<size_t>::max();
    DestructionCallback m_destructionCallback;

    std::vector<T> m_data;          // Data pool.
    std::vector<size_t> m_lookup;   // Lookup to m_data array.
    std::vector<Handle> m_holdersLookup;

public:
    ComponentDataHolder()
    {

    }

    ~ComponentDataHolder()
    {
        clear();
    }

    std::vector<T>& rawData() { return m_data; }

    T& getData(Handle handle)
    {
        return m_data[m_lookup[handle.getIdx()]];
    }

    const T& getData(Handle handle) const
    {
        return m_data[m_lookup[handle.getIdx()]];
    }

    void add(Handle handle, const T &componentData)
    {
        DF3D_ASSERT(!contains(handle));

        if (m_lookup.size() <= handle.getIdx())
            m_lookup.resize(handle.getIdx() + 1, InvalidComponentInstance);
        m_lookup[handle.getIdx()] = m_data.size();

        m_data.push_back(componentData);
        m_holdersLookup.push_back(handle);
    }

    void remove(Handle handle)
    {
        DF3D_ASSERT(m_data.size() > 0 && contains(handle));

        if (m_destructionCallback)
            m_destructionCallback(getData(handle));

        auto holderBack = m_holdersLookup.back();

        auto idx = m_lookup[handle.getIdx()];
        m_lookup[handle.getIdx()] = InvalidComponentInstance;

        if (idx != m_data.size() - 1)
            m_data[idx] = std::move(m_data.back());

        m_data.pop_back();
        m_holdersLookup.pop_back();

        if (idx < m_data.size())
        {
            m_lookup[holderBack.getIdx()] = idx;
            m_holdersLookup[idx] = holderBack;
        }
    }

    bool contains(Handle handle) const
    {
        DF3D_ASSERT(handle.isValid());
        return handle.getIdx() < m_lookup.size() && m_lookup[handle.getIdx()] != InvalidComponentInstance;
    }

    void clear()
    {
        if (m_destructionCallback)
        {
            for (auto &data : m_data)
                m_destructionCallback(data);
        }

        m_data.clear();
        m_lookup.clear();
        m_holdersLookup.clear();
    }

    void setDestructionCallback(const DestructionCallback &callback) { m_destructionCallback = callback; }
};

}
