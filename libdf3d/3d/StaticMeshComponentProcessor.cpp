#include "StaticMeshComponentProcessor.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/game/World.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/3d/SceneGraphComponentProcessor.h>
#include <libdf3d/game/ComponentDataHolder.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/render/MeshData.h>
#include <libdf3d/utils/MathUtils.h>

namespace df3d {

struct StaticMeshComponentProcessor::Impl
{
    struct Data
    {
        glm::mat4 holderTransformation;
        glm::vec3 holderPosition;
        Entity holder;
        glm::vec3 holderScale;
        shared_ptr<MeshData> meshData;
        bool visible = true;
        bool frustumCullingDisabled = false;
    };

    ComponentDataHolder<Data> data;

    static BoundingSphere getBoundingSphere(const Data &compData)
    {
        // TODO_ecs: mb move this to helpers.
        auto meshDataSphere = compData.meshData->getBoundingSphere();
        if (!meshDataSphere || !meshDataSphere->isValid())
            return BoundingSphere();

        BoundingSphere sphere = *meshDataSphere;

        // Update transformation.
        sphere.setPosition(compData.holderPosition);

        // FIXME: absolutely incorrect!!! Should take into account children.
        // TODO_ecs:

        // FIXME: wtf is this??? Why can't just scale radius?
        auto rad = sphere.getRadius() * utils::math::UnitVec3;
        rad.x *= compData.holderScale.x;
        rad.y *= compData.holderScale.y;
        rad.z *= compData.holderScale.z;
        sphere.setRadius(glm::length(rad));

        return sphere;
    }
};

void StaticMeshComponentProcessor::update()
{
    glm::quat tmp;
    // TODO_ecs: get only changed components.
    // Update the transform component.
    for (auto &compData : m_pimpl->data.rawData())
        m_world->sceneGraph().getWorldTransformMeshWorkaround(compData.holder, compData.holderTransformation, compData.holderPosition, tmp, compData.holderScale);
}

void StaticMeshComponentProcessor::draw(RenderQueue *ops)
{
    for (const auto &compData : m_pimpl->data.rawData())
    {
        if (!compData.meshData->isInitialized())
            continue;

        if (!compData.visible)
            continue;

        if (!compData.frustumCullingDisabled)
        {
            const auto &frustum = m_world->getCamera()->getFrustum();
            if (!frustum.sphereInFrustum(Impl::getBoundingSphere(compData)))
                continue;
        }

        // TODO_ecs: remove this method.
        compData.meshData->populateRenderQueue(ops, compData.holderTransformation);
    }
}

void StaticMeshComponentProcessor::cleanStep(const std::list<Entity> &deleted)
{
    m_pimpl->data.cleanStep(deleted);
}

StaticMeshComponentProcessor::StaticMeshComponentProcessor(World *world)
    : m_pimpl(new Impl()),
    m_world(world)
{

}

StaticMeshComponentProcessor::~StaticMeshComponentProcessor()
{
    //glog << "StaticMeshComponentProcessor::~StaticMeshComponentProcessor alive entities" << m_pimpl->data.rawData().size() << logdebug;
}

shared_ptr<MeshData> StaticMeshComponentProcessor::getMeshData(Entity e) const
{
    return m_pimpl->data.getData(e).meshData;
}

AABB StaticMeshComponentProcessor::getAABB(Entity e)
{
    // FIXME: mb cache if transformation hasn't been changed?
    auto &compData = m_pimpl->data.getData(e);
    // Update transformation.
    compData.holderTransformation = m_world->sceneGraph().getWorldTransform(compData.holder);

    AABB transformedAABB;

    auto modelSpaceAABB = compData.meshData->getAABB();
    if (!modelSpaceAABB || !modelSpaceAABB->isValid())
        return transformedAABB;

    // Get the corners of original AABB (model-space).
    std::vector<glm::vec3> aabbCorners(8);
    modelSpaceAABB->getCorners(aabbCorners);

    // Create new AABB from the corners of the original also applying world transformation.
    for (auto &p : aabbCorners)
    {
        auto trp = compData.holderTransformation * glm::vec4(p, 1.0f);
        transformedAABB.updateBounds(glm::vec3(trp));
    }

    return transformedAABB;
}

BoundingSphere StaticMeshComponentProcessor::getBoundingSphere(Entity e)
{
    // FIXME: mb cache if transformation hasn't been changed?
    return Impl::getBoundingSphere(m_pimpl->data.getData(e));
}

OBB StaticMeshComponentProcessor::getOBB(Entity e)
{
    // FIXME: mb cache if transformation hasn't been changed?
    assert(false && "Not implemented");
    return OBB();
}

void StaticMeshComponentProcessor::setVisible(Entity e, bool visible)
{
    m_pimpl->data.getData(e).visible = visible;
}

void StaticMeshComponentProcessor::disableFrustumCulling(Entity e, bool disable)
{
    m_pimpl->data.getData(e).frustumCullingDisabled = disable;
}

void StaticMeshComponentProcessor::add(Entity e, const std::string &meshFilePath)
{
    add(e, meshFilePath, ResourceLoadingMode::ASYNC);
}

void StaticMeshComponentProcessor::add(Entity e, const std::string &meshFilePath, ResourceLoadingMode lm)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has a static mesh component" << logwarn;
        return;
    }

    Impl::Data data;
    data.meshData = svc().resourceManager().getFactory().createMeshData(meshFilePath, lm);
    data.holder = e;
    data.holderTransformation = m_world->sceneGraph().getWorldTransform(e);

    m_pimpl->data.add(e, data);
}

void StaticMeshComponentProcessor::add(Entity e, shared_ptr<MeshData> meshData)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has a static mesh component" << logwarn;
        return;
    }

    Impl::Data data;
    data.meshData = meshData;
    data.holder = e;
    data.holderTransformation = m_world->sceneGraph().getWorldTransform(e);

    m_pimpl->data.add(e, data);
}

void StaticMeshComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        glog << "Failed to remove static mesh component from an entity. Component is not attached" << logwarn;
        return;
    }

    m_pimpl->data.remove(e);
}

bool StaticMeshComponentProcessor::has(Entity e)
{
    return m_pimpl->data.lookup(e).valid();
}

}
