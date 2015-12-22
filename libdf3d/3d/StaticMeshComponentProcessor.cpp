#include "df3d_pch.h"
#include "StaticMeshComponentProcessor.h"

#include <base/EngineController.h>
#include <game/World.h>
#include <3d/Camera.h>
#include <3d/TransformComponentProcessor.h>
#include <game/ComponentDataHolder.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>
#include <render/MeshData.h>
#include <utils/MathUtils.h>

namespace df3d {

struct StaticMeshComponentProcessor::Impl
{
    // TODO_ecs: more cache friendly layout.
    struct Data
    {
        shared_ptr<MeshData> meshData;
        Entity holder;
        glm::mat4 holderTransformation;
        glm::vec3 holderPosition;
        glm::vec3 holderScale;

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

void StaticMeshComponentProcessor::update(float systemDelta, float gameDelta)
{
    // TODO_ecs: get only changed components.
    // Update the transform component.
    for (auto &compData : m_pimpl->data.rawData())
    {
        compData.holderTransformation = world().transform().getTransformation(compData.holder);
        compData.holderPosition = world().transform().getPosition(compData.holder, true);
        compData.holderScale = world().transform().getScale(compData.holder);
    }
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
            const auto &frustum = svc().world().getCamera().getFrustum();
            if (!frustum.sphereInFrustum(Impl::getBoundingSphere(compData)))
                continue;
        }

        // TODO_ecs: remove this method.
        compData.meshData->populateRenderQueue(ops, compData.holderTransformation);
    }
}

void StaticMeshComponentProcessor::cleanStep(World &w)
{

}

StaticMeshComponentProcessor::StaticMeshComponentProcessor()
    : m_pimpl(new Impl())
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
    const auto &compData = m_pimpl->data.getData(e);

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
    data.holderTransformation = world().transform().getTransformation(e);

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

}