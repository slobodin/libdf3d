#include "df3d_pch.h"
#include "ParticleSystemComponentProcessor.h"

#include <scene/impl/ComponentDataHolder.h>
#include <scene/World.h>
#include <scene/Camera.h>
#include <scene/TransformComponentProcessor.h>
#include <base/EngineController.h>
#include <particlesys/impl/SparkInterface.h>

namespace df3d {

struct ParticleSystemComponentProcessor::Impl
{
    struct Data
    {
        SPK::Ref<SPK::System> system;

        Entity holder;
        ComponentInstance transformComponent;
        bool paused = false;
        bool worldTransformed = true;
        float systemLifeTime = -1.0f;
        float systemAge = 0.0f;
    };

    scene_impl::ComponentDataHolder<Data> data;

    static void updateCameraPosition(Data &compData)
    {
        auto spkSystem = compData.system;
        for (size_t i = 0; i < spkSystem->getNbGroups(); ++i)
        {
            if (spkSystem->getGroup(i)->isDistanceComputationEnabled())
            {
                auto pos = svc().world().getCamera().getPosition();
                if (!compData.worldTransformed)
                {
                    // Transform camera position into this node space.
                    auto invWorldTransform = world().transform().getTransformation(compData.transformComponent);
                    invWorldTransform = glm::inverse(invWorldTransform);

                    pos = glm::vec3((invWorldTransform * glm::vec4(pos, 1.0f)));
                }

                spkSystem->setCameraPosition(particlesys_impl::glmToSpk(pos));
                break;
            }
        }
    }
};

void ParticleSystemComponentProcessor::update(float systemDelta, float gameDelta)
{
    // Update the transform component idx.
    for (auto &compData : m_pimpl->data.rawData())
    {
        compData.transformComponent = world().transform().lookup(compData.holder);
        assert(compData.transformComponent.valid());
    }

    for (auto &compData : m_pimpl->data.rawData())
    {
        auto spkSystem = compData.system;
        //bool visible = world().transform().vi

        // TODO_ecs: visibility.
        //if (compData.paused || !m_holder->isVisible())
            return;

        if (compData.worldTransformed)
        {
            auto tr = world().transform().getTransformation(compData.transformComponent);

            spkSystem->getTransform().set(glm::value_ptr(tr));
        }

        Impl::updateCameraPosition(compData);
        spkSystem->updateParticles(gameDelta);

        if (compData.systemLifeTime > 0.0f)
        {
            compData.systemAge += gameDelta;
            if (compData.systemAge > compData.systemLifeTime)
            {
                // TODO_ecs:
                assert(false);
                //m_holder->detachComponent(ComponentType::PARTICLE_EFFECT);
            }
        }
    }
}

void ParticleSystemComponentProcessor::draw(RenderQueue *ops)
{
    for (const auto &compData : m_pimpl->data.rawData())
    {
        glm::mat4 transf;
        if (!compData.worldTransformed)
            transf = world().transform().getTransformation(compData.transformComponent);

        auto spkSystem = compData.system;
        // Prepare drawing of spark system.
        for (size_t i = 0; i < spkSystem->getNbGroups(); i++)
        {
            auto renderer = static_cast<particlesys_impl::ParticleSystemRenderer*>(spkSystem->getGroup(i)->getRenderer().get());
            renderer->m_currentRenderQueue = ops;
            renderer->m_currentTransformation = &transf;
        }

        spkSystem->renderParticles();

        // FIXME: do we need this?
        for (size_t i = 0; i < spkSystem->getNbGroups(); i++)
        {
            auto renderer = static_cast<particlesys_impl::ParticleSystemRenderer*>(spkSystem->getGroup(i)->getRenderer().get());
            renderer->m_currentRenderQueue = nullptr;
        }
    }
}

ParticleSystemComponentProcessor::ParticleSystemComponentProcessor()
    : m_pimpl(new Impl())
{
    // Clamp the step to 100 ms.
    SPK::System::setClampStep(true, 0.1f);
    SPK::System::useRealStep();
}

ParticleSystemComponentProcessor::~ParticleSystemComponentProcessor()
{
    SPK_DUMP_MEMORY
}

void ParticleSystemComponentProcessor::stop(ComponentInstance comp)
{
    // TODO:
    assert(false);
}

void ParticleSystemComponentProcessor::pause(ComponentInstance comp, bool paused)
{
    m_pimpl->data.getData(comp).paused = paused;
}

void ParticleSystemComponentProcessor::setSystemLifeTime(ComponentInstance comp, float lifeTime)
{
    m_pimpl->data.getData(comp).systemLifeTime = lifeTime;
}

void ParticleSystemComponentProcessor::setWorldTransformed(ComponentInstance comp, bool worldTransformed)
{
    m_pimpl->data.getData(comp).worldTransformed = worldTransformed;
}

float ParticleSystemComponentProcessor::getSystemLifeTime(ComponentInstance comp) const
{
    return m_pimpl->data.getData(comp).systemLifeTime;
}

ComponentInstance ParticleSystemComponentProcessor::add(Entity e, const std::string &vfxResource)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has a particle system component" << logwarn;
        return ComponentInstance();
    }

    Impl::Data data;

    // TODO_ecs
    data.holder = e;
    data.transformComponent = world().transform().lookup(e);
    assert(data.transformComponent.valid());

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
