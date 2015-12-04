#pragma once

#include <resources/Resource.h>
#include <render/MeshData.h>

namespace df3d {

namespace resource_loaders_impl { class MeshLoader_obj; class MeshLoader_dfmesh; }

class MeshDataManualLoader : public ManualResourceLoader
{
    std::vector<SubMesh> m_geometry;

public:
    MeshDataManualLoader(std::vector<SubMesh> &&geometry);

    MeshData* load() override;
};

class MeshDataFSLoader : public FSResourceLoader
{
    friend class resource_loaders_impl::MeshLoader_obj;
    friend class resource_loaders_impl::MeshLoader_dfmesh;

    struct Mesh
    {
        std::vector<SubMesh> submeshes;
        std::vector<unique_ptr<std::string>> materialNames;
        std::string materialLibName;
        AABB aabb;
        BoundingSphere sphere;
        OBB obb;
        ConvexHull convexHull;
    };

    std::string m_path;
    unique_ptr<Mesh> m_mesh;

public:
    MeshDataFSLoader(const std::string &path, ResourceLoadingMode lm);

    MeshData* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

}
