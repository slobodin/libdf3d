#include "df3d_pch.h"
#include "MeshComponent.h"

#include <scene/Node.h>
#include <scene/Frustum.h>
#include <scene/Camera.h>
#include <components/TransformComponent.h>
#include <render/MeshData.h>
#include <render/VertexIndexBuffer.h>
#include <render/RenderQueue.h>
#include <render/Material.h>
#include <render/RenderPass.h>
#include <render/Technique.h>
#include <resources/ResourceFactory.h>
#include <base/SystemsMacro.h>

namespace df3d { namespace components {

struct MeshComponent::ResourceMgrListenerImpl : public resources::ResourceManager::Listener
{
    std::recursive_mutex m_mutex;
    MeshComponent *m_holder;

public:
    ResourceGUID m_guid;

    ResourceMgrListenerImpl(MeshComponent *holder)
        : m_holder(holder)
    {
        g_resourceManager->addListener(this);
    }

    ~ResourceMgrListenerImpl()
    {
        g_resourceManager->removeListener(this);
    }

    void onLoadFromFileSystemRequest(ResourceGUID resourceId) override { }

    void onLoadFromFileSystemRequestComplete(ResourceGUID resourceId) override
    {
        std::lock_guard<decltype(m_mutex)> lock(m_mutex);

        if (resourceId == m_guid)
            m_holder->m_meshWasLoaded = true;
    }
};

bool MeshComponent::isInFov()
{
    if (m_frustumCullingDisabled)
        return true;

    return g_sceneManager->getCamera()->getFrustum().sphereInFrustum(getBoundingSphere());
}

void MeshComponent::onComponentEvent(components::ComponentEvent ev)
{
    switch (ev)
    {
    case components::ComponentEvent::POSITION_CHANGED:
    case components::ComponentEvent::ORIENTATION_CHANGED:
        m_transformedAabbDirty = true;
        m_obbTransformationDirty = true;
        break;
    case components::ComponentEvent::SCALE_CHANGED:
        m_transformedAabbDirty = true;
        m_obbTransformationDirty = true;
        m_boundingSphereDirty = true;
        break;
    default:
        break;
    }
}

void MeshComponent::onDraw(render::RenderQueue *ops)
{
    if (!m_meshData || !m_meshData->isInitialized())
        return;

    if (!m_visible || !isInFov())
        return;

    m_meshData->populateRenderQueue(ops, getHolder()->transform()->getTransformation());
}

void MeshComponent::onUpdate(float dt)
{
    if (m_meshWasLoaded)
    {
        m_meshWasLoaded = false;
        sendEvent(ComponentEvent::MESH_ASYNC_LOAD_COMPLETE);
    }
}

void MeshComponent::constructTransformedAABB()
{
    // TODO_REFACTO
    m_transformedAABB.reset();

    auto modelSpaceAABB = m_meshData->getAABB();
    if (!modelSpaceAABB || !modelSpaceAABB->isValid())
        return;

    // Get the corners of original AABB (model-space).
    std::vector<glm::vec3> aabbCorners(8);
    modelSpaceAABB->getCorners(aabbCorners);

    // Create new AABB from the corners of the original also applying world transformation.
    auto tr = getHolder()->transform()->getTransformation();
    for (auto &p : aabbCorners)
    {
        auto trp = tr * glm::vec4(p, 1.0f);
        m_transformedAABB.updateBounds(glm::vec3(trp));
    }

    m_transformedAabbDirty = false;
}

void MeshComponent::constructBoundingSphere()
{
    m_sphere.reset();
    auto meshDataSphere = m_meshData->getBoundingSphere();
    if (!meshDataSphere || !meshDataSphere->isValid())
        return;

    m_sphere = *meshDataSphere;

    // Update transformation.
    auto pos = m_holder->transform()->getPosition();
    auto scale = m_holder->transform()->getScale();
    m_sphere.setPosition(pos);

    // FIXME: wtf is this??? Why can't just scale radius?
    auto dir = m_sphere.getRadius() * glm::normalize(pos + glm::vec3(1.0f, 1.0f, 1.0f));
    dir = glm::mat3(glm::scale(scale)) * dir;

    m_sphere.setRadius(glm::length(dir));

    m_boundingSphereDirty = false;
}

void MeshComponent::updateBoundingSpherePosition()
{
    m_sphere.setPosition(m_holder->transform()->getPosition());
}

void MeshComponent::updateOBB()
{
    // TODO_REFACTO
    m_obb.reset();
    auto obb = m_meshData->getOBB();
    if (!obb)
        return;

    m_obb = *obb;

    m_obb.setTransformation(m_holder->transform()->getTransformation());
    m_obbTransformationDirty = false;
}

MeshComponent::MeshComponent()
    : NodeComponent(MESH),
      m_rmgrListener(make_unique<ResourceMgrListenerImpl>(this))
{

}

MeshComponent::MeshComponent(const std::string &meshFilePath, ResourceLoadingMode lm)
    : MeshComponent()
{
    m_rmgrListener->m_guid = resources::CreateGUIDFromPath(meshFilePath);

    setMeshData(g_resourceManager->getFactory().createMeshData(meshFilePath, lm));
}

MeshComponent::~MeshComponent()
{

}

void MeshComponent::setMeshData(shared_ptr<render::MeshData> geometry)
{
    if (!geometry)
    {
        base::glog << "Can not set null geometry to a mesh node" << base::logwarn;
        return;
    }

    m_meshData = geometry;
    m_meshWasLoaded = false;
    m_transformedAabbDirty = true;
    m_boundingSphereDirty = true;
    m_obbTransformationDirty = true;
}

shared_ptr<render::MeshData> MeshComponent::getMeshData() const
{
    return m_meshData;
}

scene::AABB MeshComponent::getAABB()
{
    // TODO_REFACTO
    auto modelSpaceAABB = m_meshData->getAABB();
    if (!modelSpaceAABB)
        return scene::AABB();       // No valid AABB for non valid geometry.

    if (m_transformedAabbDirty)
        constructTransformedAABB();

    return m_transformedAABB;
}

scene::BoundingSphere MeshComponent::getBoundingSphere()
{
    if (m_boundingSphereDirty)
        constructBoundingSphere();

    updateBoundingSpherePosition();

    return m_sphere;
}

scene::OBB MeshComponent::getOBB()
{
    // TODO_REFACTO
    if (m_obbTransformationDirty)
        updateOBB();

    return m_obb;
}

shared_ptr<NodeComponent> MeshComponent::clone() const
{
    assert(false);

    auto retRes = shared_ptr<MeshComponent>(new MeshComponent());

    // TODO_REFACTO
    /*

    // Clone mesh node fields.
    retRes->m_geometry = m_geometry;
    retRes->m_aabb = m_aabb;
    retRes->m_transformedAABB = m_transformedAABB;
    retRes->m_aabbDirty = m_aabbDirty;
    retRes->m_transformedAabbDirty = m_transformedAabbDirty;
    retRes->m_boundingSphereDirty = m_boundingSphereDirty;
    retRes->m_sphere = m_sphere;
    retRes->m_obb = m_obb;
    retRes->m_obbDirty = m_obbDirty;
    retRes->m_obbTransformationDirty = m_obbTransformationDirty;
    retRes->m_visible = m_visible;
    retRes->m_frustumCullingDisabled = m_frustumCullingDisabled;
    retRes->m_rmgrListener->m_guid = m_rmgrListener->m_guid;
    */

    return retRes;
}

} }
