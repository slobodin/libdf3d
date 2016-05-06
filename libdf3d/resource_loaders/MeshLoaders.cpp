#include "MeshLoaders.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/resources/ResourceFactory.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/io/FileSystemHelpers.h>
#include <libdf3d/io/FileDataSource.h>
#include <libdf3d/render/MaterialLib.h>
#include <libdf3d/utils/MeshUtils.h>

#include "impl/MeshLoader_obj.h"
#include "impl/MeshLoader_dfmesh.h"

namespace df3d {

MeshDataManualLoader::MeshDataManualLoader(std::vector<SubMesh> &&geometry)
    : m_geometry(std::move(geometry))
{

}

MeshData* MeshDataManualLoader::load()
{
    for (auto &s : m_geometry)
        utils::mesh::computeTangentBasis(s);

    auto result = new MeshData(m_geometry);
    result->m_aabb.constructFromGeometry(m_geometry);
    result->m_obb.constructFromGeometry(m_geometry);
    result->m_sphere.constructFromGeometry(m_geometry);

    return result;
}

MeshDataFSLoader::MeshDataFSLoader(const std::string &path, ResourceLoadingMode lm)
    : FSResourceLoader(lm),
    m_path(path)
{

}

MeshData* MeshDataFSLoader::createDummy()
{
    return new MeshData();
}

bool MeshDataFSLoader::decode(shared_ptr<FileDataSource> source)
{
    const auto extension = FileSystemHelpers::getFileExtension(source->getPath());

    if (extension == ".obj")
        m_mesh = resource_loaders_impl::MeshLoader_obj().load(source);
    else if (extension == ".dfmesh")
        m_mesh = resource_loaders_impl::MeshLoader_dfmesh().load(source);

    return m_mesh != nullptr;
}

void MeshDataFSLoader::onDecoded(Resource *resource)
{
    auto meshdata = static_cast<MeshData*>(resource);

    DF3D_ASSERT(m_mesh, "sanity check");

    meshdata->m_aabb = m_mesh->aabb;
    meshdata->m_obb = m_mesh->obb;
    meshdata->m_sphere = m_mesh->sphere;
    meshdata->m_convexHull = m_mesh->convexHull;

    // Load all the materials.
    auto mtlLibPath = m_mesh->materialLibName;
    mtlLibPath = FileSystemHelpers::pathConcatenate(FileSystemHelpers::getFileDirectory(m_path), mtlLibPath);

    // Leaving null material in submesh, do not set default as it will be set later. mb it's wrong design?
    if (auto mtlLib = svc().resourceManager().getFactory().createMaterialLib(mtlLibPath))
    {
        DF3D_ASSERT(m_mesh->submeshes.size() == m_mesh->materialNames.size(), "sanity check");

        for (size_t i = 0; i < m_mesh->submeshes.size(); i++)
        {
            auto &submesh = m_mesh->submeshes[i];

            if (m_mesh->materialNames[i])
                submesh.setMaterial(mtlLib->getMaterial(*m_mesh->materialNames[i]));
        }
    }

    meshdata->doInitMesh(m_mesh->submeshes);

    /*
    size_t meshTotalBytes = 0;
    for (const auto &s : m_mesh->submeshes)
        meshTotalBytes += s.getVertexData().getVerticesCount() * s.getVertexData().getFormat().getVertexSize();
    glog << "Cleaning up" << meshTotalBytes / 1024.0f << "KB of CPU copy of mesh data" << logdebug;
    */

    m_mesh.reset();     // Cleanup main memory.
}

unique_ptr<MeshDataFSLoader::Mesh> LoadMeshDataFromFile_Workaround(shared_ptr<FileDataSource> source)
{
    const auto extension = FileSystemHelpers::getFileExtension(source->getPath());

    if (extension == ".obj")
        return resource_loaders_impl::MeshLoader_obj().load(source);
    else if (extension == ".dfmesh")
        return resource_loaders_impl::MeshLoader_dfmesh().load(source);

    return nullptr;
}

}
