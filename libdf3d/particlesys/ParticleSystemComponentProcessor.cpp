#include "df3d_pch.h"
#include "ParticleSystemComponentProcessor.h"

#include <scene/impl/ComponentDataHolder.h>

namespace df3d {

struct ParticleSystemComponentProcessor::Impl
{
    struct Data
    {
        SPK::Ref<SPK::System> system;

        bool paused = false;
        bool worldTransformed = true;
        float systemLifeTime = -1.0f;
        float systemAge = 0.0f;
    };

    ComponentDataHolder<Data> data;
};

ParticleSystemComponentProcessor::ParticleSystemComponentProcessor()
    : m_pimpl(new Impl())
{

}

ParticleSystemComponentProcessor::~ParticleSystemComponentProcessor()
{

}

void ParticleSystemComponentProcessor::stop(ComponentInstance comp)
{
    // TODO:
    assert(false);
}

void ParticleSystemComponentProcessor::pause(ComponentInstance comp, bool paused)
{
    assert(comp.valid());

    m_pimpl->data.getData(comp).paused = paused;
}

void ParticleSystemComponentProcessor::setSystemLifeTime(ComponentInstance comp, float lifeTime)
{
    assert(comp.valid());

    m_pimpl->data.getData(comp).systemLifeTime = lifeTime;
}

void ParticleSystemComponentProcessor::setWorldTransformed(ComponentInstance comp, bool worldTransformed)
{
    assert(comp.valid());

    m_pimpl->data.getData(comp).worldTransformed = worldTransformed;
}

float ParticleSystemComponentProcessor::getSystemLifeTime(ComponentInstance comp) const
{
    assert(comp.valid());

    return m_pimpl->data.getData(comp).systemLifeTime;
}

ComponentInstance ParticleSystemComponentProcessor::add(Entity e, const std::string &jsonResource)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has a particle system component" << logwarn;
        return ComponentInstance();
    }

    Impl::Data data;

    // TODO_ecs

    return m_pimpl->data.add(e, data);
}

void ParticleSystemComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        glog << "Failed to remove particle system component from an entity. Component is not attached" << logwarn;
        return;
    }

    m_pimpl->data.remove(e);
}

ComponentInstance ParticleSystemComponentProcessor::lookup(Entity e)
{
    return m_pimpl->data.lookup(e);
}

}
