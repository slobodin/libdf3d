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
    std::string m_path;

public:
    MeshDataFSLoader(const std::string &path, ResourceLoadingMode lm);

    render::MeshData* createDummy(const ResourceGUID &guid) override;
    void decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

} }
