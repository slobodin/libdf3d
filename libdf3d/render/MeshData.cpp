#include "df3d_pch.h"
#include "MeshData.h"

#include <base/SystemsMacro.h>
#include <render/MaterialLib.h>
#include <resources/ResourceFactory.h>

namespace df3d { namespace render {

SubMesh::SubMesh()
{

}

SubMesh::~SubMesh()
{

}

size_t SubMesh::getVerticesCount() const
{
    return m_verticesCount;
}

void SubMesh::appendVertexData(const float *source, size_t vertexCount)
{
    // Assume that vertex format is valid.
    assert(m_vertexFormat.getVertexSize() != 0);

    auto elementsCount = m_vertexFormat.getVertexSize() * vertexCount / sizeof(float);
    // FIXME:
    // Don't use back_inserter.
    std::copy(source, source + elementsCount, std::back_inserter(m_vertexData));

    m_verticesCount += vertexCount;
}

void MeshData::doInitMesh(const std::vector<SubMesh> &geometry)
{
    assert(!isInitialized());

    // TODO_REFACTO
    for (const auto &s : geometry) 
    {
        HardwareSubMesh hs;

        // Loading materials here, is it ok?
        auto material = g_resourceManager->getFactory().createMaterialLib(s.getMtlLibPath())->getMaterial(s.getMtlName());
        if (material)
            hs.material = *material;
        else
            base::glog << "Setting up default material in" << getFilePath() << base::logwarn;


    }

    //m_aabb.constructFromGeometry()

    m_initialized = true;

    // TODO:
    // Create GPU buffers.
    // Set initialized.
    // Check sizes of cpu verts, clear mb!
}

MeshData::MeshData()
{

}

MeshData::MeshData(const std::vector<SubMesh> &geometry)
{
    doInitMesh(geometry);
}

MeshData::~MeshData()
{

}

void MeshData::setMaterial(const Material &newMaterial)
{
    if (!isInitialized())
    {
        base::glog << "Can't set material because mesh is not initialized" << base::logwarn;
        return;
    }

    for (auto &s : m_submeshes)
        s.material = newMaterial;
}

void MeshData::setMaterial(const Material &newMaterial, size_t submeshIdx)
{
    if (!isInitialized())
    {
        base::glog << "Can't set material because mesh is not initialized" << base::logwarn;
        return;
    }

    if (submeshIdx >= getSubMeshesCount())
    {
        base::glog << "Failed to set material to submesh" << submeshIdx << "because it is not exist" << base::logwarn;
        return;
    }

    m_submeshes[submeshIdx].material = newMaterial;
}

size_t MeshData::getSubMeshesCount() const
{
    if (!isInitialized())
        return 0;

    // TODO_REFACTO

    return 0;
}

size_t MeshData::getTrianglesCount() const
{
    if (!isInitialized())
        return 0;

    // TODO_REFACTO

    /*
    if (m_ib)
    {
        // Indexed.
        return m_ib->getIndices().size() / 3;
    }
    else
    {
        return m_vb->getElementsUsed() / 3;
    }
    */

    return 0;
}

const scene::AABB* MeshData::getAABB() const
{
    if (!isInitialized())
        return nullptr;

    return &m_aabb;
}

const scene::BoundingSphere* MeshData::getBoundingSphere() const
{
    if (!isInitialized())
        return nullptr;

    return &m_sphere;
}

const scene::OBB* MeshData::getOBB() const
{
    if (!isInitialized())
        return nullptr;

    return &m_obb;
}

} }
