#include "MeshLoaders.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFactory.h>
#include <df3d/engine/io/FileSystem.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/io/DataSource.h>
#include <df3d/engine/render/MaterialLib.h>
#include <df3d/engine/render/MeshUtils.h>

#include "loaders/MeshLoader_obj.h"
#include "loaders/MeshLoader_dfmesh.h"

namespace df3d {

MeshDataManualLoader::MeshDataManualLoader(std::vector<SubMesh> &&geometry)
    : m_geometry(std::move(geometry))
{

}

MeshData* MeshDataManualLoader::load()
{
    // TODO: compute tangent basis, etc?

    auto result = new MeshData(m_geometry);
    result->m_aabb.constructFromGeometry(m_geometry);
    result->m_sphere.constructFromGeometry(m_geometry);

    return result;
}

MeshDataFSLoader::Mesh::Mesh()
    : convexHull(MemoryManager::allocDefault())
{

}

MeshDataFSLoader::Mesh::~Mesh()
{

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

bool MeshDataFSLoader::decode(shared_ptr<DataSource> source)
{
    const auto extension = FileSystemHelpers::getFileExtension(source->getPath());

    if (extension == ".obj")
        m_mesh = resource_loaders::MeshLoader_obj().load(source);
    else if (extension == ".dfmesh")
        m_mesh = resource_loaders::MeshLoader_dfmesh().load(source);

    return m_mesh != nullptr;
}

void MeshDataFSLoader::onDecoded(Resource *resource)
{
    auto meshdata = static_cast<MeshData*>(resource);

    DF3D_ASSERT(m_mesh);

    meshdata->m_aabb = m_mesh->aabb;
    meshdata->m_sphere = m_mesh->sphere;
    meshdata->m_convexHull = m_mesh->convexHull;

    // Load all the materials.
    auto mtlLibPath = m_mesh->materialLibName;
    mtlLibPath = FileSystemHelpers::pathConcatenate(FileSystemHelpers::getFileDirectory(m_path), mtlLibPath);

    // Leaving null material in submesh, do not set default as it will be set later. mb it's wrong design?
    if (auto mtlLib = svc().resourceManager().getFactory().createMaterialLib(mtlLibPath))
    {
        DF3D_ASSERT(m_mesh->submeshes.size() == m_mesh->materialNames.size());

        for (size_t i = 0; i < m_mesh->submeshes.size(); i++)
        {
            auto &submesh = m_mesh->submeshes[i];

            if (m_mesh->materialNames[i])
                submesh.material = mtlLib->getMaterial(*m_mesh->materialNames[i]);
        }
    }

    meshdata->doInitMesh(m_mesh->submeshes);

    /*
    size_t meshTotalBytes = 0;
    for (const auto &s : m_mesh->submeshes)
        meshTotalBytes += s.getVertexData().getVerticesCount() * s.getVertexData().getFormat().getVertexSize();
    */

    m_mesh.reset();     // Cleanup main memory.
}

unique_ptr<MeshDataFSLoader::Mesh> LoadMeshDataFromFile_Workaround(shared_ptr<DataSource> source)
{
    const auto extension = FileSystemHelpers::getFileExtension(source->getPath());

    if (extension == ".obj")
        return resource_loaders::MeshLoader_obj().load(source);
    else if (extension == ".dfmesh")
        return resource_loaders::MeshLoader_dfmesh().load(source);

    return nullptr;
}

}
