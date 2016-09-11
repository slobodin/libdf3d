#include "MeshLoader_dfmesh.h"

#include <df3d/engine/io/DataSource.h>
#include <df3d/engine/EngineController.h>

namespace df3d { namespace resource_loaders {

MeshLoader_dfmesh::MeshLoader_dfmesh()
{

}

unique_ptr<MeshDataFSLoader::Mesh> MeshLoader_dfmesh::load(shared_ptr<DataSource> source)
{
    DFMeshHeader header;
    DataSourceGetObjects(source.get(), &header, 1);

    if (strncmp((const char *)&header.magic, "DFME", 4) != 0)
    {
        DFLOG_WARN("Invalid dfmesh magic");
        return nullptr;
    }

    if (header.version != DFMESH_VERSION)
    {
        DFLOG_WARN("Unsupported dfmesh version %d", header.version);
        return nullptr;
    }

    DF3D_ASSERT(header.indexSize == sizeof(uint32_t));

    // TODO: vertex format is hardcoded.

    auto result = make_unique<MeshDataFSLoader::Mesh>();
    result->materialLibName = header.materialLib;

    source->seek(header.submeshesOffset, SeekDir::BEGIN);

    for (int i = 0; i < header.submeshesCount; i++)
    {
        DFMeshSubmeshHeader smHeader;
        DataSourceGetObjects(source.get(), &smHeader, 1);

        const size_t verticesCount = smHeader.vertexDataSizeInBytes / sizeof(Vertex_p_n_tx_tan_bitan);
        const size_t indicesCount = smHeader.indexDataSizeInBytes / header.indexSize;

        SubMesh submesh = Vertex_p_n_tx_tan_bitan::getFormat();
        submesh.vbufferUsageType = GpuBufferUsageType::STATIC;
        submesh.ibufferUsageType = GpuBufferUsageType::STATIC;

        submesh.vertexData.addVertices(verticesCount);
        DataSourceGetObjects(source.get(), (Vertex_p_n_tx_tan_bitan*)submesh.vertexData.getRawData(), verticesCount);

        submesh.indices.resize(indicesCount);
        DataSourceGetObjects(source.get(), submesh.indices.data(), indicesCount);

        std::string materialId = smHeader.materialId;

        result->submeshes.push_back(std::move(submesh));
        if (!materialId.empty())
            result->materialNames.push_back(make_unique<std::string>(materialId));
        else
            result->materialNames.push_back(nullptr);
    }

    // TODO: deserialize bounding volumes.

    result->aabb.constructFromGeometry(result->submeshes);
    result->sphere.constructFromGeometry(result->submeshes);
    result->obb.constructFromGeometry(result->submeshes);
    result->convexHull.constructFromGeometry(result->submeshes);

    return result;
}

} }
