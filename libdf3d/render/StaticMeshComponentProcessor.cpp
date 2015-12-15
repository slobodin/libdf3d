#include "df3d_pch.h"
#include "StaticMeshComponentProcessor.h"

#include <scene/impl/ComponentDataHolder.h>
#include <base/EngineController.h>
#include <resources/ResourceManager.h>
#include <resources/ResourceFactory.h>

namespace df3d {

struct StaticMeshComponentProcessor::Impl
{
    // TODO: more cache friendly layout.
    struct Data
    {
        shared_ptr<MeshData> meshData;

        // Transformed AABB.
        AABB transformedAABB;
        bool transformedAabbDirty = true;

        // Bounding sphere.
        BoundingSphere sphere;
        bool boundingSphereDirty = true;

        // Oriented bb.
        OBB obb;
        bool obbTransformationDirty = true;

        bool visible = true;
        bool frustumCullingDisabled = false;
    };

    ComponentDataHolder<Data> data;
};

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
    return AABB();
}

BoundingSphere StaticMeshComponentProcessor::getBoundingSphere(ComponentInstance comp)
{
    return BoundingSphere();
}

OBB StaticMeshComponentProcessor::getOBB(ComponentInstance comp)
{
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
