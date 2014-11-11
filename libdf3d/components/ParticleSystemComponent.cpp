#include "df3d_pch.h"
#include "ParticleSystemComponent.h"

#include <components/TransformComponent.h>
#include <utils/ParticleSystemLoader.h>
#include <particlesys/SparkInterface.h>
#include <base/Controller.h>
#include <scene/SceneManager.h>
#include <scene/Camera.h>
#include <scene/Node.h>
#include <render/RenderOperation.h>
#include <render/RenderPass.h>
#include <render/RenderQueue.h>

namespace df3d { namespace components {

void ParticleSystemComponent::onUpdate(float dt)
{
    if (glm::epsilonEqual(dt, 0.0f, glm::epsilon<float>()))
        return;

    if (!m_system || m_paused)
        return;

    m_system->setCameraPosition(particlesys::glmToSpk(g_sceneManager->getCamera()->transform()->getPosition()));
    m_system->update(dt);

    if (m_systemLifeTime > 0.0f)
    {
        m_systemAge += dt;
        if (m_systemAge > m_systemLifeTime)
        {
            m_holder->detachComponent(CT_PARTICLE_EFFECT);
        }
    }
}

size_t ParticleSystemComponent::getGroupCount()
{
    if (!m_system || getParticlesCount() == 0)
        return 0;

    return m_renderOps.size();
}

void ParticleSystemComponent::prepareRenderOperations()
{
    if (!m_system)
        return;

    m_system->render();

    //auto holderTransform = m_holder->transform()->getTransformation();
    //auto dir = particlesys::spkToGlm(m_system->getLocalTransformLookLH());
    //auto right = particlesys::spkToGlm(m_system->getLocalTransformSide());
    //auto up = particlesys::spkToGlm(m_system->getLocalTransformUp());
    //auto pos = particlesys::spkToGlm(m_system->getLocalTransformPos());

    //auto rot = glm::mat3(right.x, right.y, right.z, up.x, up.y, up.z, dir.x, dir.y, dir.z);
    //auto myTransform = glm::mat4(rot);

    //auto resultTransform = holderTransform * myTransform;

    // FIXME:
    // Including only translation.
    
    auto tr = glm::value_ptr(m_holder->transform()->getTransformation());
    glm::mat4 worldTransf = glm::translate(glm::vec3(tr[12], tr[13], tr[14]));

    for (auto op : m_renderOps)
        op->worldTransform = m_holder->transform()->getTransformation();
}

const render::RenderOperation &ParticleSystemComponent::getRenderOperation(size_t groupIdx)
{
    render::RenderOperation *op = m_renderOps[groupIdx];

    particlesys::ParticleSystemRenderer *rend = static_cast<particlesys::ParticleSystemRenderer *>(m_system->getGroup(groupIdx)->getRenderer());
    op->vertexData = rend->getVertexBuffer();
    op->indexData = rend->getIndexBuffer();
    op->passProps = rend->m_pass;

    return *op;
}

void ParticleSystemComponent::onDraw(render::RenderQueue *ops)
{
    prepareRenderOperations();

    size_t renderOpsCount = getGroupCount();
    for (size_t i = 0; i < renderOpsCount; ++i)
    {
        const render::RenderOperation &op = getRenderOperation(i);

        if (op.passProps && op.vertexData)
        {
            if (op.passProps->isTransparent())
                ops->transparentOperations.push_back(op);
            else
                ops->notLitOpaqueOperations.push_back(op);
        }
    }
}

ParticleSystemComponent::ParticleSystemComponent()
    : NodeComponent(CT_PARTICLE_EFFECT)
{

}

ParticleSystemComponent::ParticleSystemComponent(const char *vfxDefinitionFile)
    : ParticleSystemComponent()
{
    utils::particle_system_loader::init(this, vfxDefinitionFile);
}

ParticleSystemComponent::~ParticleSystemComponent()
{
    for (auto op : m_renderOps)
        delete op;

    if (m_system)
        SPK_Destroy(m_system);
}

size_t ParticleSystemComponent::getParticlesCount() const
{
    return m_system->getNbParticles();
}

void ParticleSystemComponent::stop()
{
    if (!m_system)
        return;
    m_system->empty();
}

shared_ptr<NodeComponent> ParticleSystemComponent::clone() const
{
    auto retRes = shared_ptr<ParticleSystemComponent>(new ParticleSystemComponent());

    if (m_system)
        retRes->m_system = SPK_Copy(SPK::System, m_system);

    retRes->m_paused = m_paused;
    retRes->m_systemLifeTime = m_systemLifeTime;
    retRes->m_systemAge = m_systemAge;

    for (size_t i = 0; i < m_renderOps.size(); i++)
        retRes->m_renderOps.push_back(new render::RenderOperation());

    return retRes;
}

} }