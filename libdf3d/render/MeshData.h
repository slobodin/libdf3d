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
class DF3D_DLL SubMesh
{
    std::string m_mtlLibPath;
    std::string m_mtlName;
    
    std::vector<float> m_vertexData;
    VertexFormat m_vertexFormat;
    IndexArray m_indexData;
    GpuBufferUsageType m_vbufferUsageType = GpuBufferUsageType::STATIC;
    GpuBufferUsageType m_ibufferUsageType = GpuBufferUsageType::STATIC;

    size_t m_verticesCount = 0;

public:
    SubMesh();
    ~SubMesh();

    void setMtlLibPath(const std::string &mtlLibPath) { m_mtlLibPath = mtlLibPath; }
    void setMtlName(const std::string &mtlName) { m_mtlName = mtlName; }
    void setVertexFormat(const VertexFormat &vf) { m_vertexFormat = vf; }
    void setVertexBufferUsageHint(GpuBufferUsageType usageType) { m_vbufferUsageType = usageType; }
    void setIndexBufferUsageHint(GpuBufferUsageType usageType) { m_ibufferUsageType = usageType; }

    //! Returns material lib path associated with this submesh.
    const std::string& getMtlLibPath() const { return m_mtlLibPath; }
    //! Returns material name in the material lib.
    const std::string& getMtlName() const { return m_mtlName; }
    //! Returns vertex format of this submesh.
    const VertexFormat& getVertexFormat() const { return m_vertexFormat; }
    //! Returns count of vertices.
    size_t getVerticesCount() const { return m_verticesCount; }
    //! Returns count of indices if have any.
    size_t getIndicesCount() const { return m_indexData.size(); }
    //! Returns vertex buffer usage hint.
    GpuBufferUsageType getVertexBufferUsageHint() const { return m_vbufferUsageType; }
    //! Returns index buffer usage hint.
    GpuBufferUsageType getIndexBufferUsageHint() const { return m_ibufferUsageType; }

    // FIXME: Do not like these getters.
    const float* getVertexData() const { return m_vertexData.data(); }
    float* getVertexData() { return m_vertexData.data(); }
    const IndexArray &getIndices() const { return m_indexData; }

    bool hasIndices() const { return !m_indexData.empty(); }

    // TODO_REFACTO: mb iterator?? instead of raw vertex data.

    //! Appends vertices from given raw float source to the vertexData interpreting them using vertexFormat.
    void appendVertexData(const float *source, size_t vertexCount);
};

class DF3D_DLL MeshData : public resources::Resource
{
    friend class resources::MeshDataManualLoader;
    friend class resources::MeshDataFSLoader;

    size_t m_trianglesCount = 0;

    // TODO_REFACTO: make non copyable
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

    size_t getSubMeshesCount() const;
    size_t getTrianglesCount() const;

    const scene::AABB* getAABB() const;
    const scene::BoundingSphere* getBoundingSphere() const;
    const scene::OBB* getOBB() const;

    void populateRenderQueue(RenderQueue *ops, const glm::mat4 &transformation);
};

} }
