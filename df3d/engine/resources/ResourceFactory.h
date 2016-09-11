#pragma once

namespace df3d {

class ResourceManager;
class AudioBuffer;
class GpuProgram;
class MeshData;
class MaterialLib;
class Texture;
struct TextureInfo;
struct SubMesh;

extern const char * const SIMPLE_LIGHTING_PROGRAM_EMBED_PATH;
extern const char * const COLORED_PROGRAM_EMBED_PATH;
extern const char * const AMBIENT_PASS_PROGRAM_EMBED_PATH;

class DF3D_DLL ResourceFactory : NonCopyable
{
    ResourceManager *m_holder;

public:
    ResourceFactory(ResourceManager *holder);
    ~ResourceFactory();

    shared_ptr<AudioBuffer> createAudioBuffer(const std::string &audioPath, bool streamed = false);
    shared_ptr<GpuProgram> createGpuProgram(const std::string &vertexShader, const std::string &fragmentShader);
    shared_ptr<GpuProgram> createGpuProgram(const std::string &guid, const std::string &vertexData, const std::string &fragmentData);
    shared_ptr<GpuProgram> createSimpleLightingGpuProgram();
    shared_ptr<GpuProgram> createColoredGpuProgram();
    shared_ptr<GpuProgram> createAmbientPassProgram();
    shared_ptr<MeshData> createMeshData(const std::string &meshDataPath, ResourceLoadingMode lm);
    shared_ptr<MeshData> createMeshData(std::vector<SubMesh> &&submeshes);
    shared_ptr<Texture> createTexture(const std::string &imagePath, uint32_t flags, ResourceLoadingMode lm);
    shared_ptr<Texture> createTexture(const TextureInfo &info, const void *data, size_t dataSize);
    shared_ptr<MaterialLib> createMaterialLib(const std::string &mtlLibPath);
};

}
