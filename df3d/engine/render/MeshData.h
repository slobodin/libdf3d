#pragma once

#include <df3d/engine/resources/Resource.h>
#include <df3d/lib/math/AABB.h>
#include <df3d/lib/math/BoundingSphere.h>
#include <df3d/lib/math/OBB.h>
#include <df3d/lib/math/ConvexHull.h>
#include "Vertex.h"
#include "RenderCommon.h"
#include "RenderOperation.h"

namespace df3d {

class RenderQueue;
class MeshDataManualLoader;
class MeshDataFSLoader;
class Material;

//! Non GPU representation of a mesh surface.
struct DF3D_DLL SubMesh : NonCopyable
{
    shared_ptr<Material> material;
    VertexData vertexData;
    PodArray<uint32_t> indices;
    IndicesType indicesType;
    GpuBufferUsageType vbufferUsageType = GpuBufferUsageType::STATIC;
    GpuBufferUsageType ibufferUsageType = GpuBufferUsageType::STATIC;

    SubMesh(const VertexFormat &format);
    SubMesh(SubMesh &&other);
    ~SubMesh();
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
