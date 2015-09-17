#pragma once

#include <resources/Resource.h>
#include <scene/bounding_volumes/AABB.h>
#include <scene/bounding_volumes/BoundingSphere.h>
#include <scene/bounding_volumes/OBB.h>
#include "Vertex.h"
#include "Material.h"
#include "RenderCommon.h"

FWD_MODULE_CLASS(resources, MeshDataManualLoader)
FWD_MODULE_CLASS(resources, MeshDataFSLoader)

namespace df3d { namespace render {

class VertexBuffer;
class IndexBuffer;
class RenderQueue;

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

    void setMaterial(const Material &material) { m_material = make_shared<Material>(material); }
    void setMaterial(shared_ptr<Material> material) { m_material = material; }
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
    const VertexData& getVertexData() const { return m_vertexData; }
    VertexData& getVertexData() { return m_vertexData; }

    bool hasIndices() const { return !m_indexData.empty(); }
};

class DF3D_DLL MeshData : public resources::Resource
{
    friend class resources::MeshDataManualLoader;
    friend class resources::MeshDataFSLoader;

    size_t m_trianglesCount = 0;

    struct HardwareSubMesh
    {
        shared_ptr<VertexBuffer> vb;
        shared_ptr<IndexBuffer> ib;
        unique_ptr<Material> material;

        HardwareSubMesh() = default;
    };

    std::vector<HardwareSubMesh> m_submeshes;

    // Bounding volumes in model space.
    scene::AABB m_aabb;
    scene::BoundingSphere m_sphere;
    scene::OBB m_obb;

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

    const scene::AABB* getAABB() const;
    const scene::BoundingSphere* getBoundingSphere() const;
    const scene::OBB* getOBB() const;

    void populateRenderQueue(RenderQueue *ops, const glm::mat4 &transformation);
};

} }
