#pragma once

#include "Entity.h"
#include "World.h"
#include <utils/Utils.h>

namespace df3d {

// TODO_ecs : add test suits.
// TODO_ecs: more efficient implementation.
template<typename T>
class DF3D_DLL ComponentDataHolder : utils::NonCopyable
{
public:
    using DestructionCallback = std::function<void(const T&)>;

private:
    std::vector<T> m_data;
    // TODO_ecs: replace with array.
    // Maintain a bag of entities ids. If < 1000 for example, use array, instead - hashmap.
    std::unordered_map<Entity::IdType, ComponentInstance> m_lookup;

    DestructionCallback m_destructionCallback;

public:
    ComponentDataHolder()
    {

    }

    ~ComponentDataHolder()
    {
        clear();
    }

    ComponentInstance lookup(Entity e)
    {
        assert(e.valid());

        auto found = m_lookup.find(e.id);
        if (found == m_lookup.end())
            return ComponentInstance();

        return found->second;
    }

    void add(Entity e, const T &componentData)
    {
        assert(e.valid());
        assert(!contains(e));

        ComponentInstance inst(m_data.size());
        m_data.push_back(componentData);
        m_lookup[e.id] = inst;
    }

    void remove(Entity e)
    {
        assert(e.valid());

        auto compInstance = lookup(e);
        assert(compInstance.valid());

        if (m_destructionCallback)
            m_destructionCallback(getData(compInstance));

        m_data.erase(m_data.begin() + compInstance.id);
        m_lookup.erase(e.id);
    }

    bool contains(Entity e)
    {
        assert(e.valid());
        return utils::contains_key(m_lookup, e.id);
    }

    const T& getData(ComponentInstance inst) const
    {
        assert(inst.valid());

        return m_data[inst.id];
    }

    T& getData(ComponentInstance inst)
    {
        assert(inst.valid());

        return m_data[inst.id];
    }

    const T& getData(Entity e) const
    {
        return getData(lookup(e));
    }

    T& getData(Entity e)
    {
        return getData(lookup(e));
    }

    std::vector<T>& rawData() { return m_data; }

    void clear()
    {
        while (!m_lookup.empty())
            remove(m_lookup.begin()->first);

        assert(m_data.empty());
    }

    void cleanStep(World &w)
    {
        // TODO_ecs: more efficient removing.
        // Instead, maintain list of not alive entities.
        for (auto it = m_lookup.begin(); it != m_lookup.end(); )
        {
            if (!w.alive(it->first))
            {
                if (m_destructionCallback)
                    m_destructionCallback(getData(it->second));

                m_data.erase(m_data.begin() + it->second.id);
                it = m_lookup.erase(it);
            }
            else
                it++;
        }

        assert(m_data.size() == m_lookup.size());
    }

    void setDestructionCallback(const DestructionCallback &callback) { m_destructionCallback = callback; }
};

}
