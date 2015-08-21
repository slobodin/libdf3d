#pragma once

#include <resources/Resource.h>

namespace df3d { namespace render {

class SubMesh;
class Material;

class DF3D_DLL MeshData : public resources::Resource
{
    std::vector<shared_ptr<SubMesh>> m_submeshes;
    size_t m_trianglesCount = 0;

public:
    MeshData();
    ~MeshData();

    void attachSubMesh(shared_ptr<SubMesh> submesh);

    void clear();

    void setMaterial(shared_ptr<render::Material> newMaterial);
    void computeNormals();

    size_t getTrianglesCount() const { return m_trianglesCount; }
    const std::vector<shared_ptr<SubMesh>> &getSubMeshes() const { return m_submeshes; }

    shared_ptr<MeshData> clone() const;
};

} }
