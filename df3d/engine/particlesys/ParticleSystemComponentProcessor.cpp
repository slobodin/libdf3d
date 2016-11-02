#include "ParticleSystemComponentProcessor.h"

#include "SparkCommon.h"
#include "SparkQuadRenderer.h"
#include "ParticleSystemUtils.h"
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/game/World.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/TimeManager.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ParticleSystemResource.h>
#include <df3d/lib/JsonUtils.h>

namespace df3d {

struct ParticleSystemComponentProcessor::Impl
{
    struct Data
    {
        SPK::Ref<SPK::System> system;

        Entity holder;
        glm::mat4 holderTransform;
        bool paused = false;
        bool visible = true;
        bool worldTransformed = true;
        float systemLifeTime = -1.0f;
        float systemAge = 0.0f;
    };

    ComponentDataHolder<Data> data;
    particlesys_impl::ParticleSystemBuffers_Quad m_quadBuffers;

    static void updateCameraPosition(Data &compData, World *w)
    {
        auto spkSystem = compData.system;
        for (size_t i = 0; i < spkSystem->getNbGroups(); ++i)
        {
            if (spkSystem->getGroup(i)->isDistanceComputationEnabled())
            {
                auto pos = w->getCamera()->getPosition();
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

void ParticleSystemComponentProcessor::update()
{
    if (m_pausedGlobal)
        return;

    // Update the transform component.
    for (auto &compData : m_pimpl->data.rawData())
        compData.holderTransform = m_world->sceneGraph().getWorldTransformMatrix(compData.holder);

    auto dt = svc().timer().getFrameDelta(TIME_CHANNEL_GAME);
    for (auto &compData : m_pimpl->data.rawData())
    {
        if (compData.paused)
            continue;

        auto spkSystem = compData.system;

        if (compData.worldTransformed)
            spkSystem->getTransform().set(glm::value_ptr(compData.holderTransform));

        Impl::updateCameraPosition(compData, m_world);
        spkSystem->updateParticles(dt);

        if (compData.systemLifeTime > 0.0f)
        {
            compData.systemAge += dt;
            if (compData.systemAge > compData.systemLifeTime)
                compData.paused = true;
        }
    }
}

ParticleSystemComponentProcessor::ParticleSystemComponentProcessor(World *world)
    : m_pimpl(new Impl()),
    m_world(world)
{
    // Clamp the step to 100 ms.
    SPK::System::setClampStep(true, 0.1f);
    useRealStep();
}

ParticleSystemComponentProcessor::~ParticleSystemComponentProcessor()
{
    m_pimpl->data.clear();
}

void ParticleSystemComponentProcessor::useRealStep()
{
    SPK::System::useRealStep();
}

void ParticleSystemComponentProcessor::useConstantStep(float time)
{
    SPK::System::useConstantStep(time);
}

void ParticleSystemComponentProcessor::stop(Entity e)
{
    // TODO:
    DF3D_FATAL("not implemented");
}

void ParticleSystemComponentProcessor::pause(Entity e, bool paused)
{
    m_pimpl->data.getData(e).paused = paused;
}

void ParticleSystemComponentProcessor::pauseGlobal(bool paused)
{
    m_pausedGlobal = paused;
}

void ParticleSystemComponentProcessor::setVisible(Entity e, bool visible)
{
    m_pimpl->data.getData(e).visible = visible;
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

SPK::Ref<SPK::System> ParticleSystemComponentProcessor::getSystem(Entity e) const
{
    return m_pimpl->data.getData(e).system;
}

bool ParticleSystemComponentProcessor::isWorldTransformed(Entity e) const
{
    return m_pimpl->data.getData(e).worldTransformed;
}

bool ParticleSystemComponentProcessor::isPlaying(Entity e) const
{
    return !m_pimpl->data.getData(e).paused;
}

bool ParticleSystemComponentProcessor::isVisible(Entity e) const
{
    return !m_pimpl->data.getData(e).visible;
}

void ParticleSystemComponentProcessor::add(Entity e, ResourceID resourceID)
{
    auto resource = svc().resourceManager().getResource<ParticleSystemResource>(resourceID);
    if (resource)
        add(e, SPK::SPKObject::copy(resource->spkSystem), resource->worldTransformed, resource->systemLifeTime);
    else
        DFLOG_WARN("Can not add vfx, resource %s not found", resourceID.c_str());
}

void ParticleSystemComponentProcessor::add(Entity e, SPK::Ref<SPK::System> system, bool worldTransformed, float lifetime)
{
    if (m_pimpl->data.contains(e))
    {
        DFLOG_WARN("An entity already has a particle system component");
        return;
    }

    Impl::Data data;
    data.system = system;
    data.holder = e;
    data.systemLifeTime = lifetime;
    data.worldTransformed = worldTransformed;
    data.holderTransform = m_world->sceneGraph().getWorldTransformMatrix(e);

    m_pimpl->data.add(e, data);
}

void ParticleSystemComponentProcessor::remove(Entity e)
{
    m_pimpl->data.remove(e);
}

bool ParticleSystemComponentProcessor::has(Entity e)
{
    return m_pimpl->data.contains(e);
}

void ParticleSystemComponentProcessor::render()
{
    if (m_pausedGlobal)
        return;

    for (const auto &compData : m_pimpl->data.rawData())
    {
        if (compData.paused || !compData.visible)
            continue;

        glm::mat4 transf;
        if (!compData.worldTransformed)
            transf = compData.holderTransform;

        auto spkSystem = compData.system;
        // Prepare drawing of spark system.
        for (size_t i = 0; i < spkSystem->getNbGroups(); i++)
        {
            auto renderer = static_cast<particlesys_impl::ParticleSystemRenderer*>(spkSystem->getGroup(i)->getRenderer().get());
            renderer->m_currentTransformation = &transf;
            renderer->m_quadBuffers = &m_pimpl->m_quadBuffers;
        }

        spkSystem->renderParticles();
    }
}

}
