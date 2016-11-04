#include "StaticMeshComponentProcessor.h"

#include <df3d/engine/EngineController.h>
#include <df3d/game/World.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/game/ComponentDataHolder.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/resources/MaterialResource.h>
#include <df3d/engine/render/Material.h>
#include <df3d/engine/render/RenderOperation.h>
#include <df3d/engine/render/RenderQueue.h>
#include <df3d/lib/math/MathUtils.h>

namespace df3d {

BoundingSphere StaticMeshComponentProcessor::getBoundingSphere(const Data &compData)
{
    BoundingSphere sphere = compData.localBoundingSphere;

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

void StaticMeshComponentProcessor::update()
{
    auto &sceneGr = m_world.sceneGraph();

    // TODO: get only changed components.
    // Update the transform component.
    for (auto &compData : m_data.rawData())
        compData.holderWorldTransform = sceneGr.getWorldTransform(compData.holder);
}

void StaticMeshComponentProcessor::draw(RenderQueue *ops)
{
    if (!m_renderingEnabled)
        return;

    const auto &frustum = m_world.getCamera()->getFrustum();
    for (auto &compData : m_data.rawData())
    {
        if (!compData.visible)
            continue;

        if (!compData.frustumCullingDisabled)
        {
            if (!frustum.sphereInFrustum(getBoundingSphere(compData)))
                continue;
        }

        for (size_t i = 0; i < compData.parts.size(); i++)
        {
            auto tech = compData.materials[i].getCurrentTechnique();
            if (!tech)
                continue;

            const auto &meshPart = compData.parts[i];

            for (auto &pass : tech->passes)
            {
                RenderOperation op;
                op.vertexBuffer = meshPart.vertexBuffer;
                op.indexBuffer = meshPart.indexBuffer;
                op.numberOfElements = meshPart.numberOfElements;
                op.worldTransform = compData.holderWorldTransform.combined;
                op.passProps = &pass;

                if (op.passProps->isTransparent)
                {
                    ops->transparentOperations.push_back(op);
                }
                else
                {
                    if (op.passProps->lightingEnabled)
                        ops->litOpaqueOperations.push_back(op);
                    else
                        ops->notLitOpaqueOperations.push_back(op);
                }
            }
        }
    }
}

StaticMeshComponentProcessor::StaticMeshComponentProcessor(World &world)
    : m_world(world)
{

}

StaticMeshComponentProcessor::~StaticMeshComponentProcessor()
{

}

void StaticMeshComponentProcessor::setMaterial(Entity e, const Material &material)
{
    auto &compData = m_data.getData(e);
    for (size_t i = 0; i < compData.materials.size(); i++)
        compData.materials[i] = material;
}

void StaticMeshComponentProcessor::setMaterial(Entity e, size_t meshPartIdx, const Material &material)
{
    DF3D_FATAL("not implemented");
}

Material* StaticMeshComponentProcessor::getMaterial(Entity e, size_t meshPartIdx)
{
    auto &compData = m_data.getData(e);
    if (meshPartIdx >= compData.materials.size())
        return nullptr;
    return &compData.materials[meshPartIdx];
}

AABB StaticMeshComponentProcessor::getAABB(Entity e)
{
    // FIXME: mb cache if transformation hasn't been changed?
    auto &compData = m_data.getData(e);
    // Update transformation.
    compData.holderWorldTransform = m_world.sceneGraph().getWorldTransform(e);

    AABB transformedAABB;

    auto modelSpaceAABB = compData.localAABB;

    // Get the corners of original AABB (model-space).
    glm::vec4 aabbCorners[8];
    modelSpaceAABB.getCorners(aabbCorners);

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
    return getBoundingSphere(m_data.getData(e));
}

void StaticMeshComponentProcessor::enableRender(bool enable)
{
    m_renderingEnabled = enable;
}

void StaticMeshComponentProcessor::setVisible(Entity e, bool visible)
{
    m_data.getData(e).visible = visible;
}

void StaticMeshComponentProcessor::disableFrustumCulling(Entity e, bool disable)
{
    m_data.getData(e).frustumCullingDisabled = disable;
}

bool StaticMeshComponentProcessor::isVisible(Entity e)
{
    return m_data.getData(e).visible;
}

void StaticMeshComponentProcessor::add(Entity e, ResourceID meshResource)
{
    if (m_data.contains(e))
    {
        DFLOG_WARN("An entity already has a static mesh component");
        return;
    }

    auto &rmgr = svc().resourceManager();
    if (auto mesh = rmgr.getResource<MeshResource>(meshResource))
    {
        Data data;

        data.parts = mesh->meshParts;
        data.materials.resize(data.parts.size());
        data.localAABB = mesh->localAABB;
        data.localBoundingSphere = mesh->localBoundingSphere;

        if (!mesh->materialLibResourceId.empty())
        {
            auto materialLib = rmgr.getResource<MaterialLibResource>(mesh->materialLibResourceId);
            DF3D_ASSERT(materialLib);
            DF3D_ASSERT(mesh->meshParts.size() == mesh->materialNames.size());

            size_t idx = 0;
            for (const auto &mtlName : mesh->materialNames)
            {
                auto material = materialLib->getMaterial(mtlName);
                if (material)
                    data.materials[idx] = *material;
                ++idx;
            }
        }

        data.holder = e;
        data.holderWorldTransform = m_world.sceneGraph().getWorldTransform(e);

        m_data.add(e, data);
    }
    else
        DFLOG_WARN("Failed to add static mesh to an entity. Resource '%s' is not loaded", meshResource.c_str());
}

void StaticMeshComponentProcessor::remove(Entity e)
{
    m_data.remove(e);
}

bool StaticMeshComponentProcessor::has(Entity e)
{
    return m_data.contains(e);
}

}
