#include "ResourceFactory.h"

#include "ResourceManager.h"
#include <libdf3d/resource_loaders/TextureLoaders.h>
#include <libdf3d/resource_loaders/AudioLoaders.h>
#include <libdf3d/resource_loaders/MaterialLibLoaders.h>
#include <libdf3d/resource_loaders/MeshLoaders.h>
#include <libdf3d/render/Texture2D.h>
#include <libdf3d/render/GpuProgram.h>

namespace df3d {

const char * const SIMPLE_LIGHTING_PROGRAM_EMBED_PATH = "__embed_simple_lighting_program";
const char * const RTT_QUAD_PROGRAM_EMBED_PATH = "__embed_quad_render_program";
const char * const COLORED_PROGRAM_EMBED_PATH = "__embed_colored_program";
const char * const AMBIENT_PASS_PROGRAM_EMBED_PATH = "__embed_ambient_pass_program";

ResourceFactory::ResourceFactory(ResourceManager *holder)
    : m_holder(holder)
{

}

ResourceFactory::~ResourceFactory()
{

}

shared_ptr<AudioBuffer> ResourceFactory::createAudioBuffer(const std::string &audioPath)
{
    auto loader = make_shared<AudioBufferFSLoader>(audioPath);

    return static_pointer_cast<AudioBuffer>(m_holder->loadFromFS(audioPath, loader));
}

shared_ptr<GpuProgram> ResourceFactory::createGpuProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    // FIXME: Doesn't fit in my system(((((. Mb create 1 file with all the shaders texts of this gpu program!!!
    std::string gpuProgramId = vertexShader + ";" + fragmentShader;
    if (auto alreadyLoaded = m_holder->findResource(gpuProgramId))
        return static_pointer_cast<GpuProgram>(alreadyLoaded);

    auto loader = make_shared<GpuProgramManualLoader>(vertexShader, fragmentShader);

    return static_pointer_cast<GpuProgram>(m_holder->loadManual(loader));
}

shared_ptr<GpuProgram> ResourceFactory::createGpuProgram(const std::string &guid, const std::string &vertexData, const std::string &fragmentData)
{
    auto loader = make_shared<GpuProgramManualLoader>(guid, vertexData, fragmentData);

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

shared_ptr<GpuProgram> ResourceFactory::createRttQuadProgram()
{
    return static_pointer_cast<GpuProgram>(m_holder->findResource(RTT_QUAD_PROGRAM_EMBED_PATH));
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

    auto loader = make_shared<MeshDataManualLoader>(std::move(submeshes));

    return static_pointer_cast<MeshData>(m_holder->loadManual(loader));
}

shared_ptr<Texture2D> ResourceFactory::createTexture(const std::string &imagePath, ResourceLoadingMode lm)
{
    return createTexture(imagePath, TextureCreationParams(), lm);
}

shared_ptr<Texture2D> ResourceFactory::createTexture(const std::string &imagePath, TextureCreationParams params, ResourceLoadingMode lm)
{
    auto loader = make_shared<Texture2DFSLoader>(imagePath, params, lm);

    return static_pointer_cast<Texture2D>(m_holder->loadFromFS(imagePath, loader));
}

shared_ptr<Texture2D> ResourceFactory::createTexture(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params)
{
    auto loader = make_shared<Texture2DManualLoader>(std::move(pixelBuffer), params);

    return static_pointer_cast<Texture2D>(m_holder->loadManual(loader));
}

shared_ptr<TextureCube> ResourceFactory::createCubeTexture(const std::string &jsonPath, TextureCreationParams params, ResourceLoadingMode lm)
{
    auto loader = make_shared<TextureCubeFSLoader>(jsonPath, params, lm);
    return static_pointer_cast<TextureCube>(m_holder->loadFromFS(jsonPath, loader));
}

shared_ptr<MaterialLib> ResourceFactory::createMaterialLib(const std::string &mtlLibPath)
{
    auto loader = make_shared<MaterialLibFSLoader>(mtlLibPath);
    return static_pointer_cast<MaterialLib>(m_holder->loadFromFS(mtlLibPath, loader));
}

shared_ptr<MaterialLib> ResourceFactory::createMaterialLibFromSource(std::string &&mtlLibData)
{
    auto loader = make_shared<MaterialLibManualLoader>(std::move(mtlLibData));
    return static_pointer_cast<MaterialLib>(m_holder->loadManual(loader));
}

}
