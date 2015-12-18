#include "MeshComponent.h"

#include <scene/Node.h>
#include <scene/Frustum.h>
#include <scene/Camera.h>
#include <scene/World.h>
#include <components/TransformComponent.h>
#include <render/MeshData.h>
#include <render/VertexIndexBuffer.h>
#include <render/RenderQueue.h>
#include <render/Material.h>
#include <render/RenderPass.h>
#include <render/Technique.h>
#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>

namespace df3d {

struct MeshComponent::ResourceMgrListenerImpl : public ResourceManager::Listener
{
    std::recursive_mutex m_mutex;
    MeshComponent *m_holder;

public:
    ResourceGUID m_guid;

    ResourceMgrListenerImpl(MeshComponent *holder)
        : m_holder(holder)
    {
        svc().resourceManager().addListener(this);
    }

    ~ResourceMgrListenerImpl()
    {
        svc().resourceManager().removeListener(this);
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

    return svc().world().getCamera().getFrustum().sphereInFrustum(getBoundingSphere());
}

void MeshComponent::onComponentEvent(ComponentEvent ev)
{
    switch (ev)
    {
    case ComponentEvent::POSITION_CHANGED:
    case ComponentEvent::ORIENTATION_CHANGED:
        m_transformedAabbDirty = true;
        m_obbTransformationDirty = true;
        break;
    case ComponentEvent::SCALE_CHANGED:
        m_transformedAabbDirty = true;
        m_obbTransformationDirty = true;
        m_boundingSphereDirty = true;
        break;
    default:
        break;
    }
}

void MeshComponent::onDraw(RenderQueue *ops)
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
    m_obb.reset();
    auto obb = m_meshData->getOBB();
    if (!obb)
        return;

    m_obb = *obb;

    m_obb.setTransformation(m_holder->transform()->getTransformation());
    m_obbTransformationDirty = false;
}

void MeshComponent::setMeshData(shared_ptr<MeshData> geometry)
{
    if (!geometry)
    {
        glog << "Can not set null geometry to a mesh node" << logwarn;
        return;
    }

    m_meshData = geometry;
    m_meshWasLoaded = false;
    m_transformedAabbDirty = true;
    m_boundingSphereDirty = true;
    m_obbTransformationDirty = true;
}

MeshComponent::MeshComponent()
    : NodeComponent(ComponentType::MESH),
      m_rmgrListener(make_unique<ResourceMgrListenerImpl>(this))
{

}

MeshComponent::MeshComponent(const std::string &meshFilePath, ResourceLoadingMode lm)
    : MeshComponent()
{
    m_rmgrListener->m_guid = CreateGUIDFromPath(meshFilePath);

    setMeshData(svc().resourceManager().getFactory().createMeshData(meshFilePath, lm));
}

MeshComponent::MeshComponent(shared_ptr<MeshData> meshData)
    : MeshComponent()
{
    setMeshData(meshData);
}

MeshComponent::~MeshComponent()
{

}

shared_ptr<MeshData> MeshComponent::getMeshData() const
{
    return m_meshData;
}

AABB MeshComponent::getAABB()
{
    if (m_transformedAabbDirty)
        constructTransformedAABB();

    return m_transformedAABB;
}

BoundingSphere MeshComponent::getBoundingSphere()
{
    if (m_boundingSphereDirty)
        constructBoundingSphere();

    updateBoundingSpherePosition();

    return m_sphere;
}

OBB MeshComponent::getOBB()
{
    if (m_obbTransformationDirty)
        updateOBB();

    return m_obb;
}

shared_ptr<NodeComponent> MeshComponent::clone() const
{
    assert(false);
    auto retRes = shared_ptr<MeshComponent>(new MeshComponent());

    // Clone mesh node fields.
    retRes->m_meshData = m_meshData;
    retRes->m_transformedAABB = m_transformedAABB;
    retRes->m_transformedAabbDirty = m_transformedAabbDirty;
    retRes->m_sphere = m_sphere;
    retRes->m_boundingSphereDirty = m_boundingSphereDirty;
    retRes->m_obb = m_obb;
    retRes->m_obbTransformationDirty = m_obbTransformationDirty;
    retRes->m_visible = m_visible;
    retRes->m_frustumCullingDisabled = m_frustumCullingDisabled;
    retRes->m_rmgrListener->m_guid = m_rmgrListener->m_guid;
    retRes->m_meshWasLoaded = m_meshWasLoaded;

    return retRes;
}

}
