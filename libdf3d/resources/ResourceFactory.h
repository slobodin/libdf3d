#pragma once

FWD_MODULE_CLASS(audio, AudioBuffer)
FWD_MODULE_CLASS(render, Texture2D)
FWD_MODULE_CLASS(render, TextureCube)
FWD_MODULE_CLASS(render, GpuProgram)
FWD_MODULE_CLASS(render, MeshData)
FWD_MODULE_CLASS(render, MaterialLib)
FWD_MODULE_CLASS(render, TextureCreationParams)
FWD_MODULE_CLASS(render, PixelBuffer)

namespace df3d { namespace resources {

class ResourceManager;

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

    shared_ptr<audio::AudioBuffer> createAudioBuffer(const std::string &audioPath);
    shared_ptr<render::GpuProgram> createGpuProgram(const std::string &vertexShader, const std::string &fragmentShader);
    shared_ptr<render::GpuProgram> createGpuProgram(const std::string &guid, const std::string &vertexData, const std::string &fragmentData);
    shared_ptr<render::GpuProgram> createSimpleLightingGpuProgram();
    shared_ptr<render::GpuProgram> createColoredGpuProgram();
    shared_ptr<render::GpuProgram> createRttQuadProgram();
    shared_ptr<render::GpuProgram> createAmbientPassProgram();
    shared_ptr<render::MeshData> createMeshData(const std::string &meshDataPath, ResourceLoadingMode lm);
    // TODO_REFACTO: MeshBuilder.
    shared_ptr<render::MeshData> createMeshData();
    shared_ptr<render::Texture2D> createTexture(const std::string &imagePath, ResourceLoadingMode lm);
    shared_ptr<render::Texture2D> createTexture(const std::string &imagePath, render::TextureCreationParams params, ResourceLoadingMode lm);
    shared_ptr<render::Texture2D> createTexture(unique_ptr<render::PixelBuffer> pixelBuffer, render::TextureCreationParams params);
    shared_ptr<render::TextureCube> createCubeTexture(const std::string &positiveXImage, const std::string &negativeXImage,
                                                      const std::string &positiveYImage, const std::string &negativeYImage,
                                                      const std::string &positiveZImage, const std::string &negativeZImage,
                                                      ResourceLoadingMode lm);
    shared_ptr<render::MaterialLib> createMaterialLib(const std::string &mtlLibPath);
};

} }
