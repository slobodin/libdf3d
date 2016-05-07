#include "MeshLoader_dfmesh.h"

#include <libdf3d/io/FileDataSource.h>

namespace df3d { namespace resource_loaders_impl {

unique_ptr<SubMesh> MeshLoader_dfmesh::createSubmesh(std::vector<float> &&vertexData, IndexArray &&indexData)
{
    auto vertexFormat = VertexFormat({ VertexFormat::POSITION_3, VertexFormat::NORMAL_3,
                                     VertexFormat::TX_2, VertexFormat::COLOR_4,
                                     VertexFormat::TANGENT_3, VertexFormat::BITANGENT_3 });

    auto submesh = make_unique<SubMesh>(vertexFormat);
    submesh->setVertexBufferUsageHint(GpuBufferUsageType::STATIC);
    submesh->setIndexBufferUsageHint(GpuBufferUsageType::STATIC);

    submesh->getVertexData() = VertexData(vertexFormat, std::move(vertexData));
    submesh->getIndices() = std::move(indexData);

    return submesh;
}

MeshLoader_dfmesh::MeshLoader_dfmesh()
{

}

unique_ptr<MeshDataFSLoader::Mesh> MeshLoader_dfmesh::load(shared_ptr<FileDataSource> source)
{
    DFMeshHeader header;
    source->getAsObjects(&header, 1);

    if (strncmp((const char *)&header.magic, "DFME", 4) != 0)
    {
        glog << "Invalid dfmesh magic" << logwarn;
        return nullptr;
    }

    if (header.version != DFMESH_VERSION)
    {
        glog << "Unsupported dfmesh version" << header.version << logwarn;
        return nullptr;
    }

    if (header.indexSize != sizeof(df3d::INDICES_TYPE))
    {
        glog << "Unsupported dfmesh indices" << logwarn;
        return nullptr;
    }

    // TODO: vertex format is hardcoded.

    auto result = make_unique<MeshDataFSLoader::Mesh>();
    result->materialLibName = header.materialLib;

    source->seek(header.submeshesOffset, std::ios_base::beg);

    for (int i = 0; i < header.submeshesCount; i++)
    {
        DFMeshSubmeshHeader smHeader;
        source->getAsObjects(&smHeader, 1);

        std::vector<float> vertexData(smHeader.vertexDataSizeInBytes / sizeof(float));
        source->getAsObjects(vertexData.data(), vertexData.size());

        IndexArray indices(smHeader.indexDataSizeInBytes / header.indexSize);
        source->getAsObjects(indices.data(), indices.size());

        std::string materialId = smHeader.materialId;

        result->submeshes.push_back(std::move(*createSubmesh(std::move(vertexData), std::move(indices))));
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
