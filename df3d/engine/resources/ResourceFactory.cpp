#include "ResourceFactory.h"

#include "ResourceManager.h"
#include <df3d/engine/resources/TextureLoaders.h>
#include <df3d/engine/resources/AudioLoaders.h>
#include <df3d/engine/resources/MaterialLibLoaders.h>
#include <df3d/engine/resources/MeshLoaders.h>
#include <df3d/engine/render/GpuProgram.h>

namespace df3d {

const char * const SIMPLE_LIGHTING_PROGRAM_EMBED_PATH = "__embed_simple_lighting_program";
const char * const COLORED_PROGRAM_EMBED_PATH = "__embed_colored_program";
const char * const AMBIENT_PASS_PROGRAM_EMBED_PATH = "__embed_ambient_pass_program";

ResourceFactory::ResourceFactory(ResourceManager *holder)
    : m_holder(holder)
{

}

ResourceFactory::~ResourceFactory()
{

}

shared_ptr<AudioBuffer> ResourceFactory::createAudioBuffer(const std::string &audioPath, bool streamed)
{
    auto loader = make_shared<AudioBufferFSLoader>(audioPath, streamed);

    return static_pointer_cast<AudioBuffer>(m_holder->loadFromFS(audioPath, loader));
}

shared_ptr<GpuProgram> ResourceFactory::createGpuProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    // FIXME: Doesn't fit in my system(((((. Mb create 1 file with all the shaders texts of this gpu program!!!
    std::string gpuProgramId = vertexShader + ";" + fragmentShader;
    if (auto alreadyLoaded = m_holder->findResource(gpuProgramId))
        return static_pointer_cast<GpuProgram>(alreadyLoaded);

    GpuProgramManualLoader loader(vertexShader, fragmentShader);

    return static_pointer_cast<GpuProgram>(m_holder->loadManual(loader));
}

shared_ptr<GpuProgram> ResourceFactory::createGpuProgram(const std::string &guid, const std::string &vertexData, const std::string &fragmentData)
{
    GpuProgramManualLoader loader(guid, vertexData, fragmentData);

    return static_pointer_cast<GpuProgram>(m_holder->loadManual(loader));
}

shared_ptr<GpuProgram> ResourceFactory::createSimpleLightingGpuProgram()
{
    return static_pointer_cast<GpuProgram>(m_holder->findResource(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH));
}

shared_ptr<GpuProgram> ResourceFactory::createColoredGpuProgram()
{
    return static_pointer_cast<GpuProgram>(m_holder->findResource(COLORED_PROGRAM_EMBED_PATH));
}

shared_ptr<GpuProgram> ResourceFactory::createAmbientPassProgram()
{
    return static_pointer_cast<GpuProgram>(m_holder->findResource(AMBIENT_PASS_PROGRAM_EMBED_PATH));
}

shared_ptr<MeshData> ResourceFactory::createMeshData(const std::string &meshDataPath, ResourceLoadingMode lm)
{
    auto loader = make_shared<MeshDataFSLoader>(meshDataPath, lm);

    return static_pointer_cast<MeshData>(m_holder->loadFromFS(meshDataPath, loader));
}

shared_ptr<MeshData> ResourceFactory::createMeshData(std::vector<SubMesh> &&submeshes)
{
    // TODO: mb guid?
    MeshDataManualLoader loader(std::move(submeshes));

    return static_pointer_cast<MeshData>(m_holder->loadManual(loader));
}

shared_ptr<Texture> ResourceFactory::createTexture(const std::string &imagePath, ResourceLoadingMode lm)
{
    return createTexture(imagePath, TextureCreationParams(), lm);
}

shared_ptr<Texture> ResourceFactory::createTexture(const std::string &imagePath, TextureCreationParams params, ResourceLoadingMode lm)
{
    auto loader = make_shared<Texture2DFSLoader>(imagePath, params, lm);

    return static_pointer_cast<Texture>(m_holder->loadFromFS(imagePath, loader));
}

shared_ptr<Texture> ResourceFactory::createTexture(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params)
{
    Texture2DManualLoader loader(std::move(pixelBuffer), params);

    return static_pointer_cast<Texture>(m_holder->loadManual(loader));
}

shared_ptr<Texture> ResourceFactory::createCubeTexture(const std::string &jsonPath, TextureCreationParams params, ResourceLoadingMode lm)
{
    auto loader = make_shared<TextureCubeFSLoader>(jsonPath, params, lm);
    return static_pointer_cast<Texture>(m_holder->loadFromFS(jsonPath, loader));
}

shared_ptr<MaterialLib> ResourceFactory::createMaterialLib(const std::string &mtlLibPath)
{
    auto loader = make_shared<MaterialLibFSLoader>(mtlLibPath);
    return static_pointer_cast<MaterialLib>(m_holder->loadFromFS(mtlLibPath, loader));
}


}
