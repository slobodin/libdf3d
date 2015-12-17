#pragma once

#include <scene/Entity.h>
#include <utils/Utils.h>

namespace df3d {

// TODO_ecs : add test suits.
template<typename T>
class ComponentDataHolder : utils::NonCopyable
{
    std::vector<T> m_data;
    std::unordered_map<Entity::IdType, ComponentInstance> m_lookup;

public:
    ComponentInstance lookup(Entity e)
    {
        assert(e.valid());

        auto found = m_lookup.find(e.id);
        if (found == m_lookup.end())
            return ComponentInstance();

        return found->second;
    }

    ComponentInstance add(Entity e, const T &componentData)
    {
        assert(e.valid());
        assert(!contains(e));

        ComponentInstance inst(m_data.size());
        m_data.push_back(componentData);
        m_lookup[e.id] = inst;

        return inst;
    }

    void remove(Entity e)
    {
        assert(e.valid());
        assert(contains(e));

        // TODO_ecs
        assert(false);
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

    std::vector<T>& rawData() { return m_data; }

    void clear()
    {
        // TODO_ecs:
        assert(false);
    }
};

}
