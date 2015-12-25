#include "DebugNameComponentProcessor.h"

#include <game/ComponentDataHolder.h>

namespace df3d {

struct DebugNameComponentProcessor::Impl
{
    struct Data
    {
        Entity holder;
        std::string name;
    };

    ComponentDataHolder<Data> data;
};

void DebugNameComponentProcessor::update()
{

}

void DebugNameComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    m_pimpl->data.cleanStep(deleted);
}

DebugNameComponentProcessor::DebugNameComponentProcessor()
    : m_pimpl(new Impl())
{

}

DebugNameComponentProcessor::~DebugNameComponentProcessor()
{

}

const std::string& DebugNameComponentProcessor::getName(Entity e)
{
    return m_pimpl->data.getData(e).name;
}

Entity DebugNameComponentProcessor::getByName(const std::string &name)
{
    for (auto &data : m_pimpl->data.rawData())
    {
        if (data.name == name)
            return data.holder;
    }

    return Entity();
}

void DebugNameComponentProcessor::add(Entity e, const std::string &name)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has a debug name component" << logwarn;
        return;
    }

    Impl::Data data;
    data.name = name;
    data.holder = e;

    m_pimpl->data.add(e, data);
}

void DebugNameComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        glog << "Failed to remove debug name component from an entity. Component is not attached" << logwarn;
        return;
    }

    m_pimpl->data.remove(e);
}

bool DebugNameComponentProcessor::has(Entity e)
{
    return m_pimpl->data.lookup(e).valid();
}

}
