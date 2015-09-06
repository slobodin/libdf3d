#pragma once

#include <resources/Resource.h>
#include <scene/bounding_volumes/AABB.h>
#include <scene/bounding_volumes/BoundingSphere.h>
#include <scene/bounding_volumes/OBB.h>
#include "Vertex.h"
#include "Material.h"
#include "RenderingCapabilities.h"

namespace df3d { namespace render {

class VertexBuffer;
class IndexBuffer;

//! Non GPU representation of a mesh surface.
struct DF3D_DLL SubMesh
{
    std::string mtlLibPath;
    std::string mtlName;
    
    VertexFormat vertexFormat;
    std::vector<float> vertexData;
    IndexArray indexData;

    // TODO_REFACTO
    size_t getVerticesCount() const { assert(false); return 0; }

    // TODO_REFACTO: mb iterator??
};

class DF3D_DLL MeshData : public resources::Resource
{
    size_t m_trianglesCount = 0;

    struct HardwareSubMesh
    {
        shared_ptr<VertexBuffer> vb;
        shared_ptr<IndexBuffer> ib;
        Material material;

        std::vector<float> cpuVertices;
        VertexFormat format;
    };

    std::vector<HardwareSubMesh> m_submeshes;
    std::vector<SubMesh> m_cpuSubmeshesCopy;    // mesh surface is stored here if there is no file from where we can load this resource.

    // Bounding volumes in model space.
    scene::AABB m_aabb;
    scene::BoundingSphere m_sphere;
    scene::OBB m_obb;

    void computeNormals(const float *vertices, const VertexFormat &format);
    void computeTangentBasis();

public:
    // TODO_REFACTO - this ctor only for mesh decoder.
    // TODO_REFACTO - create ctor with cpu verts (limited ctor with only one submesh).
    MeshData();
    ~MeshData();

    // TODO_REFACTO consider move semantics
    //! Sets given material for all submeshes.
    void setMaterial(const Material &newMaterial);
    //! Sets given material for a particular submesh.
    void setMaterial(const Material &newMaterial, size_t submeshIdx);

    size_t getSubMeshesCount() const;
    size_t getTrianglesCount() const;

    const scene::AABB* getAABB() const;
    const scene::BoundingSphere* getBoundingSphere() const;
    const scene::OBB* getOBB() const;
};

} }
