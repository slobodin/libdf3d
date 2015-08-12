#include "df3d_pch.h"
#include "DecoderDFMesh.h"

#include <render/MeshData.h>
#include <render/SubMesh.h>
#include <render/Material.h>
#include <render/VertexIndexBuffer.h>

namespace df3d { namespace resources {

// File format
// HEADER
// ......
// SUBMESHES SEGMENT
// ......
// VERTEX DATA SEGMENT
// ......
// INDEX DATA SEGMENT
// ......
// MATERIALS SEGMENT
// ......
// CONVEX HULL
// ......

#pragma pack(push, 1)

struct DFMeshHeader
{
    unsigned char header[4];
    uint16_t version;

    uint16_t submeshesCount;
    uint32_t materialsOffset;
    uint32_t vertexBuffersOffset;
    uint32_t indexBuffersOffset;
    uint32_t convexHullOffset;
    uint32_t convexHullSize;

    uint8_t vertexFormat;
};

struct SubmeshHeader
{
    uint32_t vbOffset;
    uint32_t idxOffset;
    uint32_t vbSize;
    uint32_t idxSize;
    uint32_t materialOffset;
    uint32_t materialSize;
};

#pragma pack(pop)

enum class DFMeshVertexFormat
{

};

DecoderDFMesh::DecoderDFMesh()
{

}

DecoderDFMesh::~DecoderDFMesh()
{

}

shared_ptr<Resource> DecoderDFMesh::createResource()
{
    return make_shared<render::MeshData>();
}

bool DecoderDFMesh::decodeResource(shared_ptr<FileDataSource> file, shared_ptr<Resource> resource)
{
    return false;
}

} }
