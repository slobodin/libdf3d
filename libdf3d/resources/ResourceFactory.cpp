#include "df3d_pch.h"
#include "ResourceFactory.h"

#include "ResourceManager.h"
#include "loaders/TextureLoaders.h"
#include "loaders/AudioLoaders.h"
#include "loaders/MaterialLibLoaders.h"
#include "loaders/MeshLoaders.h"
#include <render/Texture2D.h>
#include <render/GpuProgram.h>

namespace df3d { namespace resources {

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

shared_ptr<audio::AudioBuffer> ResourceFactory::createAudioBuffer(const std::string &audioPath)
{
    auto loader = make_shared<AudioBufferFSLoader>(audioPath);

    return static_pointer_cast<audio::AudioBuffer>(m_holder->loadFromFS(audioPath, loader));
}

shared_ptr<render::GpuProgram> ResourceFactory::createGpuProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    // FIXME: Doesn't fit in my system(((((. Mb create 1 file with all the shaders texts of this gpu program!!!
    std::string gpuProgramId = vertexShader + ";" + fragmentShader;
    if (auto alreadyLoaded = m_holder->findResource(gpuProgramId))
        return static_pointer_cast<render::GpuProgram>(alreadyLoaded);

    auto loader = make_shared<render::GpuProgramManualLoader>(vertexShader, fragmentShader);

    return static_pointer_cast<render::GpuProgram>(m_holder->loadManual(loader));
}

shared_ptr<render::GpuProgram> ResourceFactory::createGpuProgram(const std::string &guid, const std::string &vertexData, const std::string &fragmentData)
{
    auto loader = make_shared<render::GpuProgramManualLoader>(guid, vertexData, fragmentData);

    return static_pointer_cast<render::GpuProgram>(m_holder->loadManual(loader));
}

shared_ptr<render::GpuProgram> ResourceFactory::createSimpleLightingGpuProgram()
{
    return static_pointer_cast<render::GpuProgram>(m_holder->findResource(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceFactory::createColoredGpuProgram()
{
    return static_pointer_cast<render::GpuProgram>(m_holder->findResource(COLORED_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceFactory::createRttQuadProgram()
{
    return static_pointer_cast<render::GpuProgram>(m_holder->findResource(RTT_QUAD_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceFactory::createAmbientPassProgram()
{
    return static_pointer_cast<render::GpuProgram>(m_holder->findResource(AMBIENT_PASS_PROGRAM_EMBED_PATH));
}

shared_ptr<render::MeshData> ResourceFactory::createMeshData(const std::string &meshDataPath, ResourceLoadingMode lm)
{
    auto loader = make_shared<MeshDataFSLoader>(meshDataPath, lm);

    return static_pointer_cast<render::MeshData>(m_holder->loadFromFS(meshDataPath, loader));
}

shared_ptr<render::Texture2D> ResourceFactory::createTexture(const std::string &imagePath, ResourceLoadingMode lm)
{
    return createTexture(imagePath, render::TextureCreationParams(), lm);
}

shared_ptr<render::Texture2D> ResourceFactory::createTexture(const std::string &imagePath, render::TextureCreationParams params, ResourceLoadingMode lm)
{
    auto loader = make_shared<Texture2DFSLoader>(imagePath, params, lm);

    return static_pointer_cast<render::Texture2D>(m_holder->loadFromFS(imagePath, loader));
}

shared_ptr<render::Texture2D> ResourceFactory::createTexture(unique_ptr<render::PixelBuffer> pixelBuffer, render::TextureCreationParams params)
{
    auto loader = make_shared<Texture2DManualLoader>(std::move(pixelBuffer), params);

    return static_pointer_cast<render::Texture2D>(m_holder->loadManual(loader));
}

shared_ptr<render::TextureCube> ResourceFactory::createCubeTexture(const std::string &positiveXImage, const std::string &negativeXImage,
                                                                   const std::string &positiveYImage, const std::string &negativeYImage,
                                                                   const std::string &positiveZImage, const std::string &negativeZImage,
                                                                   ResourceLoadingMode lm)
{
    return nullptr;
    /*
    auto positiveX = createTexture(positiveXImage, lm);
    auto negativeX = createTexture(negativeXImage, lm);
    auto positiveY = createTexture(positiveYImage, lm);
    auto negativeY = createTexture(negativeYImage, lm);
    auto positiveZ = createTexture(positiveZImage, lm);
    auto negativeZ = createTexture(negativeZImage, lm);

    auto textureCube = make_shared<render::TextureCube>(positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ);
    textureCube->setInitialized();
    textureCube->setGUID(positiveX->getGUID() + ";" + negativeX->getGUID() + ";" +
                         positiveY->getGUID() + ";" + negativeY->getGUID() + ";" +
                         positiveZ->getGUID() + ";" + negativeZ->getGUID() + ";");

    appendResource(textureCube);

    return textureCube;
    */
}

shared_ptr<render::MaterialLib> ResourceFactory::createMaterialLib(const std::string &mtlLibPath)
{
    auto loader = make_shared<MaterialLibFSLoader>(mtlLibPath);
    return static_pointer_cast<render::MaterialLib>(m_holder->loadFromFS(mtlLibPath, loader));
}

} }
