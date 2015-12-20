#include "df3d_pch.h"
#include "ParticleSystemComponentProcessor.h"

#include <scene/ComponentDataHolder.h>
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
        glm::mat4 holderTransform;
        bool paused = false;
        bool worldTransformed = true;
        float systemLifeTime = -1.0f;
        float systemAge = 0.0f;
    };

    ComponentDataHolder<Data> data;

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
                    auto invWorldTransform = glm::inverse(compData.holderTransform);

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
    // Update the transform component.
    for (auto &compData : m_pimpl->data.rawData())
        compData.holderTransform = world().transform().getTransformation(compData.holder);

    for (auto &compData : m_pimpl->data.rawData())
    {
        auto spkSystem = compData.system;
        //bool visible = world().transform().vi

        // TODO_ecs: visibility.
        //if (compData.paused || !m_holder->isVisible())
            return;

        if (compData.worldTransformed)
            spkSystem->getTransform().set(glm::value_ptr(compData.holderTransform));

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
            transf = compData.holderTransform;

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

void ParticleSystemComponentProcessor::cleanStep(World &w)
{

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

void ParticleSystemComponentProcessor::stop(Entity e)
{
    // TODO:
    assert(false);
}

void ParticleSystemComponentProcessor::pause(Entity e, bool paused)
{
    m_pimpl->data.getData(e).paused = paused;
}

void ParticleSystemComponentProcessor::setSystemLifeTime(Entity e, float lifeTime)
{
    m_pimpl->data.getData(e).systemLifeTime = lifeTime;
}

void ParticleSystemComponentProcessor::setWorldTransformed(Entity e, bool worldTransformed)
{
    m_pimpl->data.getData(e).worldTransformed = worldTransformed;
}

float ParticleSystemComponentProcessor::getSystemLifeTime(Entity e) const
{
    return m_pimpl->data.getData(e).systemLifeTime;
}

void ParticleSystemComponentProcessor::add(Entity e, const std::string &vfxResource)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has a particle system component" << logwarn;
        return;
    }

    Impl::Data data;

    // TODO_ecs
    data.holder = e;
    data.holderTransform = world().transform().getTransformation(e);

    m_pimpl->data.add(e, data);
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

}
