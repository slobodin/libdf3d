#include "MeshLoader_dfmesh.h"

#include <df3d/engine/io/DataSource.h>
#include <df3d/engine/EngineController.h>

namespace df3d { namespace resource_loaders {

static unique_ptr<SubMesh> CreateSubmesh(PodArray<uint8_t> &&vertexData)
{
    auto submesh = make_unique<SubMesh>(Vertex_p3_n3_tx2_tan_bitan::getFormat());
    submesh->setVertexBufferUsageHint(GpuBufferUsageType::STATIC);
    submesh->setIndexBufferUsageHint(GpuBufferUsageType::STATIC);

    submesh->getVertexData() = VertexData(Vertex_p3_n3_tx2_tan_bitan::getFormat(), std::move(vertexData));

    return submesh;
}

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

    // TODO: vertex format is hardcoded.

    auto result = make_unique<MeshDataFSLoader::Mesh>();
    result->materialLibName = header.materialLib;

    source->seek(header.submeshesOffset, SeekDir::BEGIN);

    for (int i = 0; i < header.submeshesCount; i++)
    {
        DFMeshSubmeshHeader smHeader;
        DataSourceGetObjects(source.get(), &smHeader, 1);

        PodArray<uint8_t> vertexData(MemoryManager::allocDefault());
        vertexData.resize(smHeader.vertexDataSizeInBytes);

        DataSourceGetObjects(source.get(), vertexData.data(), vertexData.size());

        std::string materialId = smHeader.materialId;

        result->submeshes.push_back(std::move(*CreateSubmesh(std::move(vertexData))));
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
