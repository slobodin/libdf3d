#include "df3d_pch.h"
#include "StaticMeshComponentProcessor.h"

#include <base/EngineController.h>
#include <scene/World.h>
#include <scene/Camera.h>
#include <scene/TransformComponentProcessor.h>
#include <scene/impl/ComponentDataHolder.h>
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
        ComponentInstance transformComponent;

        bool visible = true;
        bool frustumCullingDisabled = false;
    };

    scene_impl::ComponentDataHolder<Data> data;

    static BoundingSphere getBoundingSphere(const Data &compData)
    {
        // TODO_ecs: mb move this to helpers.
        auto meshDataSphere = compData.meshData->getBoundingSphere();
        if (!meshDataSphere || !meshDataSphere->isValid())
            return BoundingSphere();

        BoundingSphere sphere = *meshDataSphere;

        // Update transformation.
        auto pos = world().transform().getPosition(compData.transformComponent, true);
        auto scale = world().transform().getScale(compData.transformComponent);
        sphere.setPosition(pos);

        // FIXME: absolutely incorrect!!! Should take into account children.
        // TODO_ecs: 

        // FIXME: wtf is this??? Why can't just scale radius?
        auto rad = sphere.getRadius() * utils::math::UnitVec3;
        rad.x *= scale.x;
        rad.y *= scale.y;
        rad.z *= scale.z;
        sphere.setRadius(glm::length(rad));

        return sphere;
    }
};

void StaticMeshComponentProcessor::update(float systemDelta, float gameDelta)
{
    // TODO_ecs: get only changed components.
    // Update the transform component idx.
    for (auto &compData : m_pimpl->data.rawData())
    {
        compData.transformComponent = world().transform().lookup(compData.holder);
        assert(compData.transformComponent.valid());
    }
}

void StaticMeshComponentProcessor::draw(RenderQueue *ops)
{
    auto &transformProcessor = world().transform();

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
        compData.meshData->populateRenderQueue(ops, transformProcessor.getTransformation(compData.transformComponent));
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

shared_ptr<MeshData> StaticMeshComponentProcessor::getMeshData(ComponentInstance comp) const
{
    return m_pimpl->data.getData(comp).meshData;
}

AABB StaticMeshComponentProcessor::getAABB(ComponentInstance comp)
{
    // FIXME: mb cache if transformation hasn't been changed?
    const auto &compData = m_pimpl->data.getData(comp);

    AABB transformedAABB;

    auto modelSpaceAABB = compData.meshData->getAABB();
    if (!modelSpaceAABB || !modelSpaceAABB->isValid())
        return transformedAABB;

    // Get the corners of original AABB (model-space).
    std::vector<glm::vec3> aabbCorners(8);
    modelSpaceAABB->getCorners(aabbCorners);

    // Create new AABB from the corners of the original also applying world transformation.
    auto tr = world().transform().getTransformation(compData.transformComponent);
    for (auto &p : aabbCorners)
    {
        auto trp = tr * glm::vec4(p, 1.0f);
        transformedAABB.updateBounds(glm::vec3(trp));
    }

    return transformedAABB;
}

BoundingSphere StaticMeshComponentProcessor::getBoundingSphere(ComponentInstance comp)
{
    // FIXME: mb cache if transformation hasn't been changed?
    const auto &compData = m_pimpl->data.getData(comp);
    return Impl::getBoundingSphere(compData);
}

OBB StaticMeshComponentProcessor::getOBB(ComponentInstance comp)
{
    // FIXME: mb cache if transformation hasn't been changed?
    assert(false && "Not implemented");
    return OBB();
}

void StaticMeshComponentProcessor::setVisible(ComponentInstance comp, bool visible)
{
    m_pimpl->data.getData(comp).visible = visible;
}

void StaticMeshComponentProcessor::disableFrustumCulling(ComponentInstance comp, bool disable)
{
    m_pimpl->data.getData(comp).frustumCullingDisabled = disable;
}

ComponentInstance StaticMeshComponentProcessor::add(Entity e, const std::string &meshFilePath, ResourceLoadingMode lm)
{
    if (m_pimpl->data.contains(e))
    {
        glog << "An entity already has a static mesh component" << logwarn;
        return ComponentInstance();
    }

    Impl::Data data;
    data.meshData = svc().resourceManager().getFactory().createMeshData(meshFilePath, lm);
    data.holder = e;
    data.transformComponent = world().transform().lookup(e);
    assert(data.transformComponent.valid());

    return m_pimpl->data.add(e, data);
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

ComponentInstance StaticMeshComponentProcessor::lookup(Entity e)
{
    return m_pimpl->data.lookup(e);
}

}
