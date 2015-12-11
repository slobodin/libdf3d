#include "df3d_pch.h"
#include "StaticMeshComponentProcessor.h"

namespace df3d {

StaticMeshComponentProcessor::StaticMeshComponentProcessor()
{

}

StaticMeshComponentProcessor::~StaticMeshComponentProcessor()
{

}

shared_ptr<MeshData> StaticMeshComponentProcessor::getMeshData(ComponentInstance comp) const
{
    return nullptr;
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

}

void StaticMeshComponentProcessor::disableFrustumCulling(ComponentInstance comp, bool disable)
{

}

}
