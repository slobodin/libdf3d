#pragma once

#include "Entity.h"
#include <libdf3d/utils/Utils.h>

namespace df3d {

template<typename T>
class ComponentDataHolder : utils::NonCopyable
{
public:
    using DestructionCallback = std::function<void(const T&)>;

private:
    std::vector<T> m_data;

    std::vector<ComponentInstance> m_lookup;
    std::unordered_map<ComponentInstance::IdType, Entity> m_holders;

    DestructionCallback m_destructionCallback;

    void grow(Entity::IdType id)
    {
        // TODO_ecs: may become big if too much entities. Use hashmap in that case.
        m_lookup.resize((id + 1) * 2);
        if (m_lookup.size() > 1000)
            DFLOG_DEBUG("ComponentDataHolder count: %d, size KB: %d", m_lookup.size(), utils::sizeKB(m_lookup.size() * sizeof(T)));
    }

public:
    ComponentDataHolder()
    {
        m_lookup.resize(128);
    }

    ~ComponentDataHolder()
    {
        clear();
    }

    ComponentInstance lookup(Entity e) const
    {
        DF3D_ASSERT_MESS(e.valid(), "looking up invalid entity");

        if (e.id >= (Entity::IdType)m_lookup.size())
            return {};

        return m_lookup[e.id];
    }

    void add(Entity e, const T &componentData)
    {
        DF3D_ASSERT_MESS(e.valid(), "adding invalid entity");
        DF3D_ASSERT_MESS(!contains(e), "entity already present");

        if (e.id >= (Entity::IdType)m_lookup.size())
            grow(e.id);

        ComponentInstance inst(m_data.size());
        m_data.push_back(componentData);
        m_lookup[e.id] = inst;

        m_holders[inst.id] = e;
    }

    void remove(Entity e)
    {
        DF3D_ASSERT_MESS(e.valid(), "removing invalid entity");
        DF3D_ASSERT_MESS(m_data.size() == m_holders.size(), "sanity check");
        DF3D_ASSERT_MESS(m_data.size() > 0, "can't remove from empty container");

        auto compInstance = lookup(e);
        DF3D_ASSERT_MESS(compInstance.valid(), "failed to lookup entity data");

        if (m_destructionCallback)
            m_destructionCallback(getData(compInstance));

        if (m_data.size() == 1)
        {
            m_data.clear();
            m_lookup.clear();
            m_holders.clear();
        }
        else
        {
            auto lastEnt = m_holders.find(m_data.size() - 1)->second;
            std::swap(m_data[compInstance.id], m_data.back());
            m_lookup[lastEnt.id] = compInstance;
            m_holders.find(compInstance.id)->second = lastEnt;

            m_holders.erase(m_data.size() - 1);
            m_data.pop_back();
            m_lookup[e.id] = {};
        }
    }

    bool contains(Entity e)
    {
        DF3D_ASSERT_MESS(e.valid(), "invalid entity");
        return lookup(e).valid();
    }

    const T& getData(ComponentInstance inst) const
    {
        DF3D_ASSERT_MESS(inst.valid(), "invalid component instance");

        return m_data[inst.id];
    }

    T& getData(ComponentInstance inst)
    {
        DF3D_ASSERT_MESS(inst.valid(), "invalid component instance");

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

    size_t getSize() const
    {
        DF3D_ASSERT_MESS(m_data.size() == m_holders.size(), "sanity check");

        return m_data.size();
    }

    void clear()
    {
        if (m_destructionCallback)
        {
            for (auto &data : m_data)
                m_destructionCallback(data);
        }

        m_data.clear();
        m_holders.clear();
        m_lookup.clear();
    }

    void cleanStep(const std::list<Entity> &deleted)
    {
        for (auto e : deleted)
        {
            if (contains(e))
                remove(e);
        }

        // TODO_ecs: shrinking to fit each n sec.
    }

    void setDestructionCallback(const DestructionCallback &callback) { m_destructionCallback = callback; }
};

}
