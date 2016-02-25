#pragma once

namespace df3d {

class ResourceManager;
class AudioBuffer;
class Texture2D;
class TextureCube;
class GpuProgram;
class MeshData;
class MaterialLib;
class TextureCreationParams;
class PixelBuffer;
class SubMesh;

extern const char * const SIMPLE_LIGHTING_PROGRAM_EMBED_PATH;
extern const char * const RTT_QUAD_PROGRAM_EMBED_PATH;
extern const char * const COLORED_PROGRAM_EMBED_PATH;
extern const char * const AMBIENT_PASS_PROGRAM_EMBED_PATH;

class DF3D_DLL ResourceFactory : utils::NonCopyable
{
    ResourceManager *m_holder;

public:
    ResourceFactory(ResourceManager *holder);
    ~ResourceFactory();

    shared_ptr<AudioBuffer> createAudioBuffer(const std::string &audioPath);
    shared_ptr<GpuProgram> createGpuProgram(const std::string &vertexShader, const std::string &fragmentShader);
    shared_ptr<GpuProgram> createGpuProgram(const std::string &guid, const std::string &vertexData, const std::string &fragmentData);
    shared_ptr<GpuProgram> createSimpleLightingGpuProgram();
    shared_ptr<GpuProgram> createColoredGpuProgram();
    shared_ptr<GpuProgram> createRttQuadProgram();
    shared_ptr<GpuProgram> createAmbientPassProgram();
    shared_ptr<MeshData> createMeshData(const std::string &meshDataPath, ResourceLoadingMode lm);
    shared_ptr<MeshData> createMeshData(std::vector<SubMesh> &&submeshes);
    shared_ptr<Texture2D> createTexture(const std::string &imagePath, ResourceLoadingMode lm);
    shared_ptr<Texture2D> createTexture(const std::string &imagePath, TextureCreationParams params, ResourceLoadingMode lm);
    shared_ptr<Texture2D> createTexture(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params);
    shared_ptr<TextureCube> createCubeTexture(const std::string &jsonPath, TextureCreationParams params, ResourceLoadingMode lm);
    shared_ptr<MaterialLib> createMaterialLib(const std::string &mtlLibPath);
    shared_ptr<MaterialLib> createMaterialLibFromSource(std::string &&mtlLibData);
};

}
