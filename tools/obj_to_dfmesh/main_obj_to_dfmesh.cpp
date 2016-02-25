#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <libdf3d/df3d.h>
#include <libdf3d/resource_loaders/MeshLoaders.h>
#include <libdf3d/resource_loaders/impl/MeshLoader_obj.h>
#include <libdf3d/resource_loaders/impl/MeshLoader_dfmesh.h>

static_assert(sizeof(typename std::string::value_type) == 1, "Invalid string size");

template<typename T>
void Serialize(const T &data, std::ofstream &fs)
{
    fs.write(reinterpret_cast<const char *>(&data), sizeof(data));
}

void Serialize(const void *data, size_t size, std::ofstream &fs)
{
    if (size == 0)
        return;
    fs.write(reinterpret_cast<const char *>(data), size);
}

df3d::resource_loaders_impl::DFMeshSubmeshHeader CreateSubmeshChunk(const df3d::SubMesh &sm, const std::string &materialId)
{
    using namespace df3d::resource_loaders_impl;

    if (materialId.size() >= DFMESH_MAX_MATERIAL_ID)
        throw std::runtime_error("material id is too big");

    DFMeshSubmeshHeader submeshChunk;
    memset(&submeshChunk, 0, sizeof(submeshChunk));

    if (!materialId.empty())
        memcpy(submeshChunk.materialId, materialId.c_str(), materialId.size());

    submeshChunk.vertexDataSizeInBytes = sm.getVertexData().getSizeInBytes();
    submeshChunk.indexDataSizeInBytes = sm.getIndices().size() * sizeof(df3d::INDICES_TYPE);

    submeshChunk.chunkSize = sizeof(DFMeshSubmeshHeader) + submeshChunk.vertexDataSizeInBytes + submeshChunk.indexDataSizeInBytes;

    return submeshChunk;
}

df3d::resource_loaders_impl::DFMeshMaterialHeader CreateMaterialDataChunk(const std::string &materialData)
{
    using namespace df3d::resource_loaders_impl;

    DFMeshMaterialHeader materialChunk;
    materialChunk.chunkSize = sizeof(DFMeshMaterialHeader) + materialData.size();
    materialChunk.materialDataSizeInBytes = materialData.size();

    return materialChunk;
}

void ProcessMesh(const df3d::MeshDataFSLoader::Mesh &meshInput, df3d::FileSystem *fs, const std::string &outputFilename)
{
    using namespace df3d::resource_loaders_impl;

    std::cout << "creating dfmesh..." << std::endl;

    // Prepare submeshes headers.
    std::vector<DFMeshSubmeshHeader> submeshHeaders(meshInput.submeshes.size());
    if (meshInput.submeshes.size() != meshInput.materialNames.size())
        throw std::runtime_error("broken mesh input");

    size_t submeshChunksSize = 0;
    for (size_t i = 0; i < meshInput.submeshes.size(); i++)
    {
        std::string materialId = meshInput.materialNames[i] ? *meshInput.materialNames[i] : "";
        submeshHeaders[i] = CreateSubmeshChunk(meshInput.submeshes[i], materialId);

        submeshChunksSize += submeshHeaders[i].chunkSize;
    }

    auto mtlLib = fs->openFile(meshInput.materialLibName);
    if (!mtlLib)
        throw std::runtime_error("failed to open material lib file");

    std::string materialData;
    materialData.resize(mtlLib->getSize());
    mtlLib->getRaw(&materialData[0], mtlLib->getSize());

    DFMeshMaterialHeader mtlLibHeader = CreateMaterialDataChunk(materialData);

    std::ofstream output(outputFilename, std::ios::out | std::ios::binary);
    if (!output)
        throw std::runtime_error("failed to open output file");

    if (meshInput.submeshes.size() > 0xFFFF)
        throw std::runtime_error("too many submeshes");

    DFMeshHeader header;
    memset(&header, 0, sizeof(header));

    header.magic = *((uint32_t*)DFMESH_MAGIC);
    header.version = DFMESH_VERSION;

    header.vertexFormat = 0;                                    // TODO
    header.indexSize = sizeof(df3d::INDICES_TYPE);
    header.submeshesCount = (uint16_t)meshInput.submeshes.size();
    header.submeshesOffset = sizeof(header);
    header.materialLibOffset = sizeof(header) + submeshChunksSize;
    header.boundingVolumesOffset = header.materialLibOffset + mtlLibHeader.chunkSize;

    // Write the header.
    Serialize(header, output);

    // Write submeshes.
    for (size_t i = 0; i < meshInput.submeshes.size(); i++)
    {
        Serialize(submeshHeaders[i], output);
        Serialize(meshInput.submeshes[i].getVertexData().getRawData(), submeshHeaders[i].vertexDataSizeInBytes, output);
        Serialize(meshInput.submeshes[i].getIndices().data(), submeshHeaders[i].indexDataSizeInBytes, output);
    }

    // Write material data.
    Serialize(mtlLibHeader, output);
    Serialize(materialData.data(), materialData.size(), output);

    if (output.fail() || output.bad())
        throw std::runtime_error("failed to write to an output");

    output.close();
}

int main(int argc, const char **argv) try
{
    std::cout << "obj_to_dfmesh starting" << std::endl;

    if (argc != 2)
        throw std::runtime_error("Invalid input. Usage: obj_to_dfmesh.exe mesh.obj");

    df3d::FileSystem fs;

    std::string inputFileName = argv[1];

    auto file = fs.openFile(inputFileName);
    if (!file)
        throw std::runtime_error("Failed to open input file");

    std::cout << "decoding obj file..." << std::endl;

    auto meshInput = df3d::resource_loaders_impl::MeshLoader_obj(&fs).load(file);
    if (!meshInput)
        throw std::runtime_error("Failed to load input obj mesh");

    std::cout << "obj successfully decoded" << std::endl;

    auto dotPos = inputFileName.find_last_of('.');
    std::string outputFilename(inputFileName.begin(), inputFileName.begin() + dotPos);

    ProcessMesh(*meshInput, &fs, outputFilename + ".dfmesh");

    std::cout << "Done!" << std::endl;

    return 0;
}
catch (std::exception &e)
{
    std::cerr << "An error occurred:\n" << e.what() << "\n";

    // TODO: remove result file if any.

    return 1;
}
