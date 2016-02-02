#include "MeshData.h"

#include "MaterialLib.h"
#include "VertexIndexBuffer.h"
#include "RenderQueue.h"
#include "Technique.h"
#include "RenderPass.h"
#include <libdf3d/base/EngineController.h>

namespace df3d {

SubMesh::SubMesh(const VertexFormat &format)
    : m_vertexData(format)
{

}

SubMesh::SubMesh(SubMesh &&other)
    : m_material(std::move(other.m_material)),
    m_vertexData(std::move(other.m_vertexData)),
    m_indexData(std::move(other.m_indexData)),
    m_vbufferUsageType(other.m_vbufferUsageType),
    m_ibufferUsageType(other.m_ibufferUsageType),
    m_verticesCount(other.m_verticesCount)
{

}

SubMesh::~SubMesh()
{

}

void SubMesh::setMaterial(const Material &material)
{
    m_material = make_shared<Material>(material);
}

void SubMesh::setMaterial(shared_ptr<Material> material)
{
    m_material = material;
}

void MeshData::doInitMesh(const std::vector<SubMesh> &geometry)
{
    assert(!isInitialized());

    m_trianglesCount = 0;

    for (const auto &s : geometry) 
    {
        m_submeshes.push_back(HardwareSubMesh());
        HardwareSubMesh &hs = m_submeshes.back();

        if (auto mtl = s.getMaterial())
        {
            hs.material = make_unique<Material>(*mtl);
        }
        else
        {
            glog << "Setting up default material in" << getFilePath() << logwarn;
            hs.material = make_unique<Material>(getFilePath());
        }

        hs.vb = make_shared<VertexBuffer>(s.getVertexData().getFormat());
        hs.vb->alloc(s.getVertexData(), s.getVertexBufferUsageHint());

        if (s.hasIndices())
        {
            hs.ib = make_shared<IndexBuffer>();
            hs.ib->alloc(s.getIndices().size(), s.getIndices().data(), s.getIndexBufferUsageHint());

            m_trianglesCount += s.getIndices().size() / 3;
        }
        else
        {
            m_trianglesCount += s.getVertexData().getVerticesCount() / 3;
        }
    }
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
        glog << "Can't set material because mesh is not initialized" << logwarn;
        return;
    }

    for (auto &s : m_submeshes)
        *s.material = newMaterial;
}

void MeshData::setMaterial(const Material &newMaterial, size_t submeshIdx)
{
    if (!isInitialized())
    {
        glog << "Can't set material because mesh is not initialized" << logwarn;
        return;
    }

    if (submeshIdx >= getSubMeshesCount())
    {
        glog << "Failed to set material to submesh" << submeshIdx << "because it does not exist" << logwarn;
        return;
    }

    *m_submeshes[submeshIdx].material = newMaterial;
}

Material& MeshData::getMaterial(size_t submeshIdx)
{
    assert(isInitialized());

    return *m_submeshes.at(submeshIdx).material;
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

const AABB* MeshData::getAABB() const
{
    if (!isInitialized())
        return nullptr;

    return &m_aabb;
}

const BoundingSphere* MeshData::getBoundingSphere() const
{
    if (!isInitialized())
        return nullptr;

    return &m_sphere;
}

const OBB* MeshData::getOBB() const
{
    if (!isInitialized())
        return nullptr;

    return &m_obb;
}

const ConvexHull* MeshData::getConvexHull() const
{
    if (!isInitialized())
        return nullptr;

    return &m_convexHull;
}

void MeshData::populateRenderQueue(RenderQueue *ops, const glm::mat4 &transformation)
{
    // Include all the submeshes.
    for (auto &sm : m_submeshes)
    {
        auto tech = sm.material->getCurrentTechnique();
        if (!tech || !sm.vb)
            continue;

        size_t passCount = tech->getPassCount();
        for (size_t passidx = 0; passidx < passCount; passidx++)
        {
            RenderOperation newoperation;
            auto passProps = tech->getPass(passidx);

            newoperation.worldTransform = transformation;
            newoperation.vertexData = sm.vb;
            newoperation.indexData = sm.ib;
            newoperation.passProps = passProps;

            if (passProps->isTransparent())
            {
                ops->transparentOperations.push_back(newoperation);
            }
            else
            {
                if (passProps->isLit())
                    ops->litOpaqueOperations.push_back(newoperation);
                else
                    ops->notLitOpaqueOperations.push_back(newoperation);
            }
        }
    }
}

}
