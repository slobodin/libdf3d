#include "MeshData.h"

#include "MaterialLib.h"
#include "RenderQueue.h"
#include "Technique.h"
#include "RenderPass.h"
#include "IRenderBackend.h"
#include "RenderManager.h"
#include <df3d/engine/EngineController.h>

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
    DF3D_ASSERT_MESS(!isInitialized(), "mesh is already initialized");

    m_trianglesCount = 0;

    for (const auto &s : geometry) 
    {
        RenderOperation op;

        if (auto mtl = s.getMaterial())
        {
            m_submeshMaterials.push_back(make_unique<Material>(*mtl));
        }
        else
        {
            DFLOG_WARN("Setting up default material in %s", getFilePath().c_str());
            m_submeshMaterials.push_back(make_unique<Material>(getFilePath()));
        }

        const auto &vertexData = s.getVertexData();

        op.vertexBuffer = svc().renderManager().getBackend().createVertexBuffer(vertexData, s.getVertexBufferUsageHint());

        if (s.hasIndices())
        {
            size_t indicesCount = s.getIndices().size();
            const void *indexData = s.getIndices().data();
            auto ibUsageHint = s.getIndexBufferUsageHint();

            op.indexBuffer = svc().renderManager().getBackend().createIndexBuffer(indicesCount, indexData, ibUsageHint);
            op.numberOfElements = indicesCount;

            m_trianglesCount += s.getIndices().size() / 3;
        }
        else
        {
            op.numberOfElements = vertexData.getVerticesCount();
            m_trianglesCount += s.getVertexData().getVerticesCount() / 3;
        }

        m_submeshes.push_back(op);
    }
}

MeshData::MeshData()
    : m_convexHull(MemoryManager::allocDefault())
{

}

MeshData::MeshData(const std::vector<SubMesh> &geometry)
    : MeshData()
{
    doInitMesh(geometry);
}

MeshData::~MeshData()
{
    for (const auto &hs : m_submeshes)
    {
        svc().renderManager().getBackend().destroyVertexBuffer(hs.vertexBuffer);
        if (hs.indexBuffer.valid())
            svc().renderManager().getBackend().destroyIndexBuffer(hs.indexBuffer);
    }
}

void MeshData::setMaterial(const Material &newMaterial)
{
    if (!isInitialized())
    {
        DFLOG_WARN("Can't set material because mesh is not initialized");
        return;
    }

    for (auto &s : m_submeshMaterials)
        *s = newMaterial;
}

void MeshData::setMaterial(const Material &newMaterial, size_t submeshIdx)
{
    if (!isInitialized())
    {
        DFLOG_WARN("Can't set material because mesh is not initialized");
        return;
    }

    if (submeshIdx >= getSubMeshesCount())
    {
        DFLOG_WARN("Failed to set material to submesh %d because it does not exist", submeshIdx);
        return;
    }

    *m_submeshMaterials[submeshIdx] = newMaterial;
}

Material& MeshData::getMaterial(size_t submeshIdx)
{
    DF3D_ASSERT_MESS(isInitialized(), "mesh should be initialized");

    return *m_submeshMaterials.at(submeshIdx);
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
    DF3D_ASSERT(m_submeshes.size() == m_submeshMaterials.size());

    // Include all the submeshes.
    for (size_t i = 0; i < m_submeshes.size(); i++)
    {
        auto tech = m_submeshMaterials[i]->getCurrentTechnique();
        if (!tech)
            continue;

        size_t passCount = tech->getPassCount();
        for (size_t passidx = 0; passidx < passCount; passidx++)
        {
            auto passProps = tech->getPass(passidx);
            auto &op = m_submeshes[i];

            op.worldTransform = transformation;
            op.passProps = passProps.get();

            if (passProps->isTransparent())
            {
                ops->transparentOperations.push_back(op);
            }
            else
            {
                if (passProps->isLit())
                    ops->litOpaqueOperations.push_back(op);
                else
                    ops->notLitOpaqueOperations.push_back(op);
            }
        }
    }
}

}
