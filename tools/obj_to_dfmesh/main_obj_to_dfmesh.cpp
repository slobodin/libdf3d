#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <df3d/df3d.h>
#include <df3d/engine/resources/loaders/MeshLoader_obj.h>
#include <df3d/engine/resources/loaders/MeshLoader_dfmesh.h>

static_assert(sizeof(typename std::string::value_type) == 1, "Invalid string size");

static const int INDICES_SIZE = sizeof(uint16_t);

template<typename T>
void Serialize(const T &data, std::ofstream &fs)
{
    fs.write(reinterpret_cast<const char *>(&data), sizeof(data));

    if (!fs)
        throw std::runtime_error("failed to write to an output");
}

void Serialize(const void *data, size_t size, std::ofstream &fs)
{
    if (size == 0)
        return;
    fs.write(reinterpret_cast<const char *>(data), size);

    if (!fs)
        throw std::runtime_error("failed to write to an output");
}

df3d::DFMeshSubmeshHeader CreateMeshPartHeader(const df3d::MeshResourceData::Part &sm)
{
    if (sm.materialName.size() >= df3d::DFMESH_MAX_MATERIAL_ID)
        throw std::runtime_error("material id is too big");

    df3d::DFMeshSubmeshHeader submeshChunk;
    memset(&submeshChunk, 0, sizeof(submeshChunk));

    memcpy(submeshChunk.materialId, sm.materialName.c_str(), sm.materialName.size());

    submeshChunk.vertexDataSizeInBytes = sm.vertexData.getSizeInBytes();
    submeshChunk.indexDataSizeInBytes = sm.indexData.size() * INDICES_SIZE;

    submeshChunk.chunkSize =
        sizeof(df3d::DFMeshSubmeshHeader) +
        submeshChunk.vertexDataSizeInBytes +
        submeshChunk.indexDataSizeInBytes;

    return submeshChunk;
}

void ProcessMesh(const df3d::MeshResourceData &meshInput, const std::string &outputFilename)
{
    if (meshInput.parts.size() > 0xFFFF)
        throw std::runtime_error("too many mesh parts");

    // Prepare submeshes headers.
    std::vector<df3d::DFMeshSubmeshHeader> submeshHeaders(meshInput.parts.size());

    size_t submeshChunksSize = 0;
    for (size_t i = 0; i < meshInput.parts.size(); i++)
    {
        submeshHeaders[i] = CreateMeshPartHeader(*meshInput.parts[i]);

        submeshChunksSize += submeshHeaders[i].chunkSize;
    }

    std::ofstream output(outputFilename, std::ios::out | std::ios::binary);
    if (!output)
        throw std::runtime_error("failed to open output file");

    df3d::DFMeshHeader header;
    memset(&header, 0, sizeof(header));

    header.magic = *((uint32_t*)df3d::DFMESH_MAGIC);
    header.version = df3d::DFMESH_VERSION;
    header.vertexFormat = 0;    // TODO
    header.indexSize = INDICES_SIZE;
    header.submeshesCount = (uint16_t)meshInput.parts.size();
    header.submeshesOffset = sizeof(header);

    // Write the header.
    Serialize(header, output);

    // Write submeshes.
    for (size_t i = 0; i < meshInput.parts.size(); i++)
    {
        Serialize(submeshHeaders[i], output);
        Serialize(meshInput.parts[i]->vertexData.getRawData(), submeshHeaders[i].vertexDataSizeInBytes, output);
        Serialize(meshInput.parts[i]->indexData.data(), submeshHeaders[i].indexDataSizeInBytes, output);
    }

    if (output.fail() || output.bad())
        throw std::runtime_error("failed to write to an output");

    output.close();
}

int main(int argc, const char **argv) try
{
    if (argc != 2)
        throw std::runtime_error("Invalid input. Usage: obj_to_dfmesh.exe mesh.obj");

    std::cout << argv[1] << "\n";

    df3d::MemoryManager::init();
    df3d::EngineCVars::objIndexize = true;
    auto &alloc = df3d::MemoryManager::allocDefault();

    auto fs = df3d::CreateDefaultResourceFileSystem();

    std::string inputFileName = argv[1];

    auto file = fs->open(inputFileName.c_str());
    if (!file)
        throw std::runtime_error("Failed to open input file");

    auto meshInput = MeshLoader_obj(*file, alloc);
    if (!meshInput)
        throw std::runtime_error("Failed to load input obj mesh");

    fs->close(file);

    // Support only 16-bit vertices for now. Some hardware doesn't work with 32 bit (Mali 400)

    for (auto part : meshInput->parts)
    {
        if (part->vertexData.getVerticesCount() >= 0xFFFF)
        {
            std::ostringstream errMsg;
            errMsg << "To much vertices: " << part->vertexData.getVerticesCount()
                << ". File: " << inputFileName;

            throw std::runtime_error(errMsg.str());
        }
    }

    auto dotPos = inputFileName.find_last_of('.');
    std::string outputFilename(inputFileName.begin(), inputFileName.begin() + dotPos);

    ProcessMesh(*meshInput, outputFilename + ".dfmesh");

    for (auto part : meshInput->parts)
        MAKE_DELETE(alloc, part);
    MAKE_DELETE(alloc, meshInput);

    fs.reset();
    df3d::MemoryManager::shutdown();

    return 0;
}
catch (std::exception &e)
{
    std::cerr << "An error occurred:\n" << e.what() << "\n";

    // TODO: remove result file if any.

    return 1;
}
