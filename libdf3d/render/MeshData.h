#pragma once

#include <libdf3d/resources/Resource.h>
#include <libdf3d/math/AABB.h>
#include <libdf3d/math/BoundingSphere.h>
#include <libdf3d/math/OBB.h>
#include <libdf3d/math/ConvexHull.h>
#include "Vertex.h"
#include "RenderCommon.h"
#include "RenderOperation.h"

namespace df3d {

class RenderQueue;
class MeshDataManualLoader;
class MeshDataFSLoader;
class Material;

//! Non GPU representation of a mesh surface.
class DF3D_DLL SubMesh : utils::NonCopyable
{
    shared_ptr<Material> m_material;
    VertexData m_vertexData;
    IndexArray m_indexData;
    GpuBufferUsageType m_vbufferUsageType = GpuBufferUsageType::STATIC;
    GpuBufferUsageType m_ibufferUsageType = GpuBufferUsageType::STATIC;

    size_t m_verticesCount = 0;

public:
    SubMesh(const VertexFormat &format);
    SubMesh(SubMesh &&other);
    ~SubMesh();

    void setMaterial(const Material &material);
    void setMaterial(shared_ptr<Material> material);
    void setVertexBufferUsageHint(GpuBufferUsageType usageType) { m_vbufferUsageType = usageType; }
    void setIndexBufferUsageHint(GpuBufferUsageType usageType) { m_ibufferUsageType = usageType; }

    //! Returns submesh material.
    shared_ptr<Material> getMaterial() const { return m_material; }
    //! Returns vertex buffer usage hint.
    GpuBufferUsageType getVertexBufferUsageHint() const { return m_vbufferUsageType; }
    //! Returns index buffer usage hint.
    GpuBufferUsageType getIndexBufferUsageHint() const { return m_ibufferUsageType; }

    // FIXME: Do not like these getters.
    const IndexArray& getIndices() const { return m_indexData; }
    IndexArray& getIndices() { return m_indexData; }
    const VertexData& getVertexData() const { return m_vertexData; }
    VertexData& getVertexData() { return m_vertexData; }

    bool hasIndices() const { return !m_indexData.empty(); }
};

class DF3D_DLL MeshData : public Resource
{
    friend class MeshDataManualLoader;
    friend class MeshDataFSLoader;

    size_t m_trianglesCount = 0;

    std::vector<RenderOperation> m_submeshes;
    std::vector<unique_ptr<Material>> m_submeshMaterials;

    // Bounding volumes in model space.
    AABB m_aabb;
    BoundingSphere m_sphere;
    OBB m_obb;
    ConvexHull m_convexHull;

    void doInitMesh(const std::vector<SubMesh> &geometry);

    MeshData();
    MeshData(const std::vector<SubMesh> &geometry);

public:
    ~MeshData();

    //! Sets given material for all submeshes.
    void setMaterial(const Material &newMaterial);
    //! Sets given material for a particular submesh.
    void setMaterial(const Material &newMaterial, size_t submeshIdx);

    Material& getMaterial(size_t submeshIdx);

    size_t getSubMeshesCount() const;
    size_t getTrianglesCount() const;

    const AABB* getAABB() const;
    const BoundingSphere* getBoundingSphere() const;
    const OBB* getOBB() const;
    const ConvexHull* getConvexHull() const;

    void populateRenderQueue(RenderQueue *ops, const glm::mat4 &transformation);
};

}
