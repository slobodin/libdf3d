#include "df3d_pch.h"
#include "MeshLoaders.h"

#include <base/SystemsMacro.h>
#include <resources/FileDataSource.h>

#include "MeshLoader_obj.h"
#include "MeshLoader_dfmesh.h"

namespace df3d { namespace resources {

MeshDataManualLoader::MeshDataManualLoader(std::vector<render::SubMesh> &&geometry)
    : m_geometry(std::move(geometry))
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
    auto extension = g_fileSystem->getFileExtension(source->getPath());

    if (extension == ".obj")
        m_mesh = MeshLoader_obj().load(source);
    else if (extension == ".dfmesh")
        m_mesh = MeshLoader_dfmesh().load(source);
}

void MeshDataFSLoader::onDecoded(Resource *resource)
{
    auto meshdata = static_cast<render::MeshData*>(resource);

    assert(m_mesh);

    meshdata->m_aabb = m_mesh->aabb;
    meshdata->m_obb = m_mesh->obb;
    meshdata->m_sphere = m_mesh->sphere;

    meshdata->doInitMesh(m_mesh->submeshes);

    m_mesh.reset();     // Cleanup main memory.

    // TODO_REFACTO:
    // Check hardware size!
}

} }
