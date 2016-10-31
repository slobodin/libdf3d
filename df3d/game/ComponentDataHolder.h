#pragma once

#include <df3d/game/Entity.h>

namespace df3d {

template<typename T>
class ComponentDataHolder : NonCopyable
{
public:
    using DestructionCallback = std::function<void(const T&)>;

private:
    const size_t InvalidComponentInstance = std::numeric_limits<size_t>::max();
    DestructionCallback m_destructionCallback;

    std::vector<T> m_data;          // Data pool.
    std::vector<size_t> m_lookup;   // Lookup to m_data array.
    std::vector<Entity> m_holdersLookup;

public:
    ComponentDataHolder()
    {

    }

    ~ComponentDataHolder()
    {
        clear();
    }

    std::vector<T>& rawData() { return m_data; }

    T& getData(Entity ent)
    {
        return m_data[m_lookup[ent.getIndex()]];
    }

    const T& getData(Entity ent) const
    {
        return m_data[m_lookup[ent.getIndex()]];
    }

    void add(Entity ent, const T &componentData)
    {
        DF3D_ASSERT(!contains(ent));

        if (m_lookup.size() <= ent.getIndex())
            m_lookup.resize(ent.getIndex() + 1, InvalidComponentInstance);
        m_lookup[ent.getIndex()] = m_data.size();

        m_data.push_back(componentData);
        m_holdersLookup.push_back(ent);
    }

    void remove(Entity ent)
    {
        DF3D_ASSERT(m_data.size() > 0 && contains(ent));

        if (m_destructionCallback)
            m_destructionCallback(getData(ent));

        auto holderBack = m_holdersLookup.back();

        auto idx = m_lookup[ent.getIndex()];
        m_lookup[ent.getIndex()] = InvalidComponentInstance;

        if (idx != m_data.size() - 1)
            m_data[idx] = std::move(m_data.back());

        m_data.pop_back();
        m_holdersLookup.pop_back();

        if (idx < m_data.size())
        {
            m_lookup[holderBack.getIndex()] = idx;
            m_holdersLookup[idx] = holderBack;
        }
    }

    bool contains(Entity ent) const
    {
        DF3D_ASSERT(ent.isValid());
        return ent.getIndex() < m_lookup.size() && m_lookup[ent.getIndex()] != InvalidComponentInstance;
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
