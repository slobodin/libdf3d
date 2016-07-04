#include "StaticMeshComponentProcessor.h"

#include <df3d/engine/EngineController.h>
#include <df3d/game/World.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFactory.h>
#include <df3d/engine/render/MeshData.h>
#include <df3d/lib/math/MathUtils.h>

namespace df3d {

struct StaticMeshComponentProcessor::Impl
{
    struct Data
    {
        Transform holderWorldTransform;
        Entity holder;
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
            return {};

        BoundingSphere sphere = *meshDataSphere;

        // Update transformation.
        sphere.setPosition(compData.holderWorldTransform.position);

        // FIXME: absolutely incorrect!!! Should take into account children.
        // TODO_ecs:

        // FIXME: wtf is this??? Why can't just scale radius?
        auto rad = sphere.getRadius() * MathUtils::UnitVec3;
        rad.x *= compData.holderWorldTransform.scaling.x;
        rad.y *= compData.holderWorldTransform.scaling.y;
        rad.z *= compData.holderWorldTransform.scaling.z;
        sphere.setRadius(glm::length(rad));

        return sphere;
    }
};

void StaticMeshComponentProcessor::update()
{
    // TODO_ecs: get only changed components.
    // Update the transform component.
    for (auto &compData : m_pimpl->data.rawData())
        compData.holderWorldTransform = m_world->sceneGraph().getWorldTransform(compData.holder);
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
        compData.meshData->populateRenderQueue(ops, compData.holderWorldTransform.combined);
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
    compData.holderWorldTransform = m_world->sceneGraph().getWorldTransform(e);

    AABB transformedAABB;

    auto modelSpaceAABB = compData.meshData->getAABB();
    if (!modelSpaceAABB || !modelSpaceAABB->isValid())
        return transformedAABB;

    // Get the corners of original AABB (model-space).
    glm::vec4 aabbCorners[8];
    modelSpaceAABB->getCorners(aabbCorners);

    // Create new AABB from the corners of the original also applying world transformation.
    for (const auto &p : aabbCorners)
    {
        auto trp = compData.holderWorldTransform.combined * p;
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
    DF3D_ASSERT_MESS(false, "not implemented");
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

bool StaticMeshComponentProcessor::isVisible(Entity e)
{
    return m_pimpl->data.getData(e).visible;
}

void StaticMeshComponentProcessor::add(Entity e, const std::string &meshFilePath)
{
    add(e, meshFilePath, ResourceLoadingMode::ASYNC);
}

void StaticMeshComponentProcessor::add(Entity e, const std::string &meshFilePath, ResourceLoadingMode lm)
{
    add(e, svc().resourceManager().getFactory().createMeshData(meshFilePath, lm));
}

void StaticMeshComponentProcessor::add(Entity e, shared_ptr<MeshData> meshData)
{
    if (m_pimpl->data.contains(e))
    {
        DFLOG_WARN("An entity already has a static mesh component");
        return;
    }

    Impl::Data data;
    data.meshData = meshData;
    data.holder = e;
    data.holderWorldTransform = m_world->sceneGraph().getWorldTransform(e);

    m_pimpl->data.add(e, data);
}

void StaticMeshComponentProcessor::remove(Entity e)
{
    if (!m_pimpl->data.contains(e))
    {
        DFLOG_WARN("Failed to remove static mesh component from an entity. Component is not attached");
        return;
    }

    m_pimpl->data.remove(e);
}

bool StaticMeshComponentProcessor::has(Entity e)
{
    return m_pimpl->data.lookup(e).valid();
}

}
