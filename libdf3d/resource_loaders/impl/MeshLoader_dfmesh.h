#pragma once

#include "../MeshLoaders.h"

namespace df3d {

class FileSystem;

namespace resource_loaders_impl {

const int DFMESH_MAX_MATERIAL_ID = 128;
const int DFMESH_MAX_MATERIAL_LIB_ID = 128;
const char DFMESH_MAGIC[4] = { 'D', 'F', 'M', 'E' };
const uint16_t DFMESH_VERSION = 1;

// File format:
// |----------------------|
// | DFMeshHeader         |
// |----------------------|
// | DFMeshSubmeshHeader  |
// | chunk vertex data    |
// | chunk index data     |
// |----------------------|
// | DFMeshSubmeshChunk   |
// | etc ...              |
// |----------------------|
// | DFMeshMaterialHeader |
// | material data        |
// | ---------------------|
// TODO: bounding volumes.

#pragma pack(push, 1)

struct DFMeshHeader
{
    uint32_t magic;
    uint16_t version;

    uint64_t vertexFormat;      // 64 attrib per vertex max.
    uint32_t indexSize;

    // ! Number of submesh chunks.
    uint16_t submeshesCount;
    //! Offset to submeshes data relative to the start.
    uint32_t submeshesOffset;
    //! Material lib path.
    char materialLib[DFMESH_MAX_MATERIAL_LIB_ID];
};

struct DFMeshSubmeshHeader
{
    uint32_t chunkSize;
    uint32_t vertexDataSizeInBytes;
    uint32_t indexDataSizeInBytes;

    char materialId[DFMESH_MAX_MATERIAL_ID];

    // Vertex data goes after the header.
    // Index data then.
};

#pragma pack(pop)

//! DFMesh file format decoder.
/*!
 * DFMesh is a file format for meshes used in libdf3d.
 */
class MeshLoader_dfmesh
{
    unique_ptr<SubMesh> createSubmesh();

public:
    MeshLoader_dfmesh();

    unique_ptr<MeshDataFSLoader::Mesh> load(shared_ptr<FileDataSource> source);
};

} }
