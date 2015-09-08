#include "df3d_pch.h"
#include "MeshLoaders.h"

namespace df3d { namespace resources {

MeshDataManualLoader::MeshDataManualLoader(std::vector<render::SubMesh> &&geometry)
    : m_geometry(geometry)
{

}

render::MeshData* MeshDataManualLoader::load()
{
    return new render::MeshData(m_geometry);
}

MeshDataFSLoader::MeshDataFSLoader(const std::string &path, ResourceLoadingMode lm)
    : FSResourceLoader(lm),
    m_path(path)
{

}

render::MeshData* MeshDataFSLoader::createDummy(const ResourceGUID &guid)
{
    return new render::MeshData();
}

void MeshDataFSLoader::decode(shared_ptr<FileDataSource> source)
{

}

void MeshDataFSLoader::onDecoded(Resource *resource)
{
    auto meshdata = static_cast<render::MeshData*>(resource);
}

} }
