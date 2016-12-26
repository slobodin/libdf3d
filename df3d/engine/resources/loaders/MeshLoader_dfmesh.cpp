#include "MeshLoader_dfmesh.h"

#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/MeshResource.h>
#include <df3d/engine/render/Vertex.h>

namespace df3d {

VertexFormat VertexFormat_dfmesh(uint16_t id)
{
    // TODO:
    if (id == 1)
        return Vertex_p_tx_c::getFormat();
    else
        return Vertex_p_n_tx_tan_bitan::getFormat();
}

MeshResourceData* MeshLoader_dfmesh(ResourceDataSource &dataSource, Allocator &alloc)
{
    DFMeshHeader header;
    dataSource.getObjects(&header, 1);

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
    auto vf = VertexFormat_dfmesh(header.vertexFormat);

    auto result = MAKE_NEW(alloc, MeshResourceData)();

    dataSource.seek(header.submeshesOffset, SeekDir::BEGIN);

    for (int i = 0; i < header.submeshesCount; i++)
    {
        DFMeshSubmeshHeader smHeader;
        dataSource.getObjects(&smHeader, 1);

        const size_t verticesCount = smHeader.vertexDataSizeInBytes / vf.getVertexSize();
        const size_t indicesCount = smHeader.indexDataSizeInBytes / header.indexSize;

        auto meshPart = MAKE_NEW(alloc, MeshResourceData::Part)(vf, alloc);
        meshPart->vertexData.addVertices(verticesCount);
        dataSource.getObjects((uint8_t*)meshPart->vertexData.getRawData(), meshPart->vertexData.getSizeInBytes());

        meshPart->indices.resize(indicesCount);
        dataSource.getObjects(meshPart->indices.data(), indicesCount);

        meshPart->materialName = Id(smHeader.materialId);

        result->parts.push_back(meshPart);
    }

    return result;
}

}
