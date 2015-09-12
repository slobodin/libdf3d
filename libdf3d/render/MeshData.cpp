#include "df3d_pch.h"
#include "MeshData.h"

#include "MaterialLib.h"
#include "VertexIndexBuffer.h"
#include <base/SystemsMacro.h>
#include <resources/ResourceFactory.h>

namespace df3d { namespace render {

SubMesh::SubMesh()
{

}

SubMesh::~SubMesh()
{

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

    m_trianglesCount = 0;

    for (const auto &s : geometry) 
    {
        m_submeshes.push_back(HardwareSubMesh());
        HardwareSubMesh &hs = m_submeshes.back();

        // Loading materials here, is it ok?
        auto material = g_resourceManager->getFactory().createMaterialLib(s.getMtlLibPath())->getMaterial(s.getMtlName());
        if (material)
        {
            hs.material = make_unique<Material>(*material);
        }
        else
        {
            base::glog << "Setting up default material in" << getFilePath() << base::logwarn;
            hs.material = make_unique<Material>(getFilePath());
        }

        hs.vb = make_shared<VertexBuffer>(s.getVertexFormat());
        hs.vb->alloc(s.getVerticesCount(), s.getVertexData(), s.getVertexBufferUsageHint());

        if (s.hasIndices())
        {
            hs.ib = make_shared<IndexBuffer>();
            hs.ib->alloc(s.getIndicesCount(), s.getIndices().data(), s.getIndexBufferUsageHint());

            m_trianglesCount += s.getIndicesCount() / 3;
        }
        else
        {
            m_trianglesCount += s.getVerticesCount() / 3;
        }
    }

    m_initialized = true;
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
        *s.material = newMaterial;
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

    *m_submeshes[submeshIdx].material = newMaterial;
}

size_t MeshData::getSubMeshesCount() const
{
    if (!isInitialized())
        return 0;

    return m_submeshes.size();
}

size_t MeshData::getTrianglesCount() const
{
    if (!isInitialized())
        return 0;

    return m_trianglesCount;
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
