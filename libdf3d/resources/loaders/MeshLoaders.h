#pragma once

#include <resources/Resource.h>
#include <render/MeshData.h>

namespace df3d { namespace resources {

class MeshDataManualLoader : public ManualResourceLoader
{
    std::vector<render::SubMesh> m_geometry;

public:
    MeshDataManualLoader(std::vector<render::SubMesh> &&geometry);

    render::MeshData* load() override;
};

class MeshDataFSLoader : public FSResourceLoader
{
    friend class MeshLoader_obj;
    friend class MeshLoader_dfmesh;

    struct Mesh
    {
        std::vector<render::SubMesh> submeshes;
        std::vector<std::unique_ptr<std::string>> materialNames;
        std::string materialLibName;
        scene::AABB aabb;
        scene::BoundingSphere sphere;
        scene::OBB obb;

        // TODO:
        // Add convex hull also.
    };

    std::string m_path;
    std::unique_ptr<Mesh> m_mesh;

public:
    MeshDataFSLoader(const std::string &path, ResourceLoadingMode lm);

    render::MeshData* createDummy() override;
    void decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

} }
