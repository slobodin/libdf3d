#pragma once

#include <df3d/engine/resources/Resource.h>
#include <df3d/engine/render/MeshData.h>

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
public:
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

private:
    std::string m_path;
    unique_ptr<Mesh> m_mesh;

public:
    MeshDataFSLoader(const std::string &path, ResourceLoadingMode lm);

    MeshData* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

// FIXME: workaround for physics. Figure out how to do this more clear way.
unique_ptr<MeshDataFSLoader::Mesh> LoadMeshDataFromFile_Workaround(shared_ptr<FileDataSource> source);

}
