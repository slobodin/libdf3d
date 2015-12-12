#pragma once

#include <scene/Entity.h>
#include <utils/Utils.h>

namespace df3d {

template<typename T>
class ComponentDataHolder : utils::NonCopyable
{
    std::vector<T> m_data;
    std::unordered_map<Entity::IdType, ComponentInstance> m_lookup;

public:
    ComponentInstance lookup(Entity e)
    {
        auto found = m_lookup.find(e.id);
        if (found == m_lookup.end())
            return ComponentInstance();

        return found->second;
    }

    void add(Entity e, const T &componentData)
    {
        assert(!contains(e));

        ComponentInstance inst(m_data.size());
        m_data.push_back(componentData);
        m_lookup[e.id] = inst;
    }

    void remove(Entity e)
    {
        assert(contains(e));

        // TODO_ecs
        assert(false);
    }

    bool contains(Entity e)
    {
        return utils::contains_key(m_lookup, e.id);
    }

    const T& getData(ComponentInstance inst) const { return m_data[inst.id]; }
    T& getData(ComponentInstance inst) { return m_data[inst.id]; }

    const T* rawData() const { return m_data.data(); }
    T* rawData() { return m_data.data(); }
};

}
