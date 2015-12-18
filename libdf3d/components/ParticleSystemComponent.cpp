#include "ParticleSystemComponent.h"

#include <components/TransformComponent.h>
#include <particlesys/impl/SparkInterface.h>
#include <scene/Camera.h>
#include <scene/World.h>
#include <scene/Node.h>
#include <render/RenderOperation.h>
#include <render/RenderQueue.h>
#include <base/EngineController.h>

namespace df3d {

void ParticleSystemComponent::updateCameraPosition()
{
    for (size_t i = 0; i < SPKSystem->getNbGroups(); ++i)
    {
        if (SPKSystem->getGroup(i)->isDistanceComputationEnabled())
        {
            auto pos = svc().world().getCamera().getPosition();
            if (!m_worldTransformed)
            {
                // Transform camera position into this node space.
                auto invWorldTransform = m_holder->transform()->getTransformation();
                invWorldTransform = glm::inverse(invWorldTransform);

                pos = glm::vec3((invWorldTransform * glm::vec4(pos, 1.0f)));
            }

            SPKSystem->setCameraPosition(particlesys_impl::glmToSpk(pos));
            break;
        }
    }
}

void ParticleSystemComponent::onUpdate(float dt)
{
    if (glm::epsilonEqual(dt, 0.0f, glm::epsilon<float>()))
        return;

    if (!SPKSystem || m_paused || !m_holder->isVisible())
        return;

    if (m_worldTransformed)
    {
        auto tr = m_holder->transform()->getTransformation();

        SPKSystem->getTransform().set(glm::value_ptr(tr));
    }

    updateCameraPosition();
    SPKSystem->updateParticles(dt);

    if (m_systemLifeTime > 0.0f)
    {
        m_systemAge += dt;
        if (m_systemAge > m_systemLifeTime)
        {
            m_holder->detachComponent(ComponentType::PARTICLE_EFFECT);
        }
    }
}

void ParticleSystemComponent::onDraw(RenderQueue *ops)
{
    glm::mat4 transf;
    if (!m_worldTransformed)
        transf = m_holder->transform()->getTransformation();

    // Prepare drawing of spark system.
    for (size_t i = 0; i < getNbSPKGroups(); i++)
    {
        auto renderer = static_cast<particlesys_impl::ParticleSystemRenderer*>(SPKSystem->getGroup(i)->getRenderer().get());
        renderer->m_currentRenderQueue = ops;
        renderer->m_currentTransformation = &transf;
    }

    SPKSystem->renderParticles();

    // FIXME: do we need this?
    for (size_t i = 0; i < getNbSPKGroups(); i++)
    {
        auto renderer = static_cast<particlesys_impl::ParticleSystemRenderer*>(SPKSystem->getGroup(i)->getRenderer().get());
        renderer->m_currentRenderQueue = nullptr;
    }
}

ParticleSystemComponent::ParticleSystemComponent()
    : NodeComponent(ComponentType::PARTICLE_EFFECT),
    SPKSystem(SPK::System::create(false))
{
    //m_worldTransformed = false;
}

ParticleSystemComponent::~ParticleSystemComponent()
{
}

size_t ParticleSystemComponent::getParticlesCount() const
{
    return SPKSystem->getNbParticles();
}

void ParticleSystemComponent::stop()
{
    if (!SPKSystem)
        return;
    // TODO:
    assert(false);
    //m_system->empty();
}

shared_ptr<NodeComponent> ParticleSystemComponent::clone() const
{
    auto retRes = shared_ptr<ParticleSystemComponent>(new ParticleSystemComponent());

    if (SPKSystem)
        retRes->SPKSystem = SPK::SPKObject::copy(SPKSystem);

    retRes->m_paused = m_paused;
    retRes->m_worldTransformed = m_worldTransformed;
    retRes->m_systemLifeTime = m_systemLifeTime;
    retRes->m_systemAge = m_systemAge;

    return retRes;
}

}
