#include "df3d_pch.h"
#include "ResourceManager.h"

#include <base/SystemsMacro.h>
#include <base/ThreadPool.h>
#include <audio/AudioBuffer.h>
#include <render/GpuProgram.h>
#include <render/Shader.h>
#include <render/Texture2D.h>
#include <render/TextureCube.h>
#include <render/MeshData.h>
#include <render/MaterialLib.h>
#include "Resource.h"
#include "FileDataSource.h"
#include "decoders/DecoderOBJ.h"
#include "decoders/DecoderMTL.h"
#include "decoders/DecoderTexture.h"
#include "decoders/DecoderTerrain.h"
#include "decoders/DecoderWAV.h"
#include "decoders/DecoderOGG.h"

namespace df3d { namespace resources {

const char * const SIMPLE_LIGHTING_PROGRAM_EMBED_PATH = "__embed_simple_lighting_program";
const char * const FFP2D_PROGRAM_EMBED_PATH = "__embed_ffp2d_program";
const char * const RTT_QUAD_PROGRAM_EMBED_PATH = "__embed_quad_render_program";
const char * const COLORED_PROGRAM_EMBED_PATH = "__embed_colored_program";
const char * const AMBIENT_PASS_PROGRAM_EMBED_PATH = "__embed_ambient_pass_program";

void ResourceManager::loadEmbedResources()
{
    auto loadInternal = [&](const char *name, const std::string &dataVert, const std::string &dataFrag)
    {
        auto program = make_shared<render::GpuProgram>();
        auto vertexShader = make_shared<render::Shader>(render::Shader::Type::VERTEX);
        vertexShader->setShaderData(dataVert);
        auto fragmentShader = make_shared<render::Shader>(render::Shader::Type::FRAGMENT);
        fragmentShader->setShaderData(dataFrag);

        program->attachShader(vertexShader);
        program->attachShader(fragmentShader);

        program->setGUID(name);
        program->setInitialized();
        // Pin forever.
        program->setResident();

        appendResource(program);
    };

    const std::string simple_lighting_vert =
#include "render/embed_glsl/simple_lighting_vert.h"
        ;
    const std::string simple_lighting_frag =
#include "render/embed_glsl/simple_lighting_frag.h"
        ;
    const std::string ffp2d_vert =
#include "render/embed_glsl/ffp2d_vert.h"
        ;
    const std::string ffp2d_frag =
#include "render/embed_glsl/ffp2d_frag.h"
        ;
    const std::string rtt_quad_vert =
#include "render/embed_glsl/rtt_quad_vert.h"
        ;
    const std::string rtt_quad_frag =
#include "render/embed_glsl/rtt_quad_frag.h"
        ;
    const std::string colored_vert =
#include "render/embed_glsl/colored_vert.h"
        ;
    const std::string colored_frag =
#include "render/embed_glsl/colored_frag.h"
        ;
    const std::string ambient_vert =
#include "render/embed_glsl/ambient_vert.h"
        ;
    const std::string ambient_frag =
#include "render/embed_glsl/ambient_frag.h"
        ;

    loadInternal(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH, simple_lighting_vert, simple_lighting_frag);
    loadInternal(FFP2D_PROGRAM_EMBED_PATH, ffp2d_vert, ffp2d_frag);
    loadInternal(RTT_QUAD_PROGRAM_EMBED_PATH, rtt_quad_vert, rtt_quad_frag);
    loadInternal(COLORED_PROGRAM_EMBED_PATH, colored_vert, colored_frag);
    loadInternal(AMBIENT_PASS_PROGRAM_EMBED_PATH, ambient_vert, ambient_frag);
}

void ResourceManager::doRequest(DecodeRequest req)
{
    // If 1 thread, need to be locked.
    //std::lock_guard<std::recursive_mutex> lock(m_lock);

    //base::glog << "Start load" << req.fileSource->getPath() << base::logmess;

    auto fileSource = g_fileSystem->openFile(req.filePath.c_str());

    bool decodeResult = req.decoder->decodeResource(fileSource, req.resource);
    req.resource->setInitialized(true);

    if (m_listener)
        m_listener->onLoadFromFileSystemRequestComplete(req.resource->getGUID());

    //base::glog << "Done load" << req.fileSource->getPath() << "with result" << decodeResult << base::logmess;
}

ResourceManager::ResourceManager()
{
    m_threadPool = make_unique<base::ThreadPool>(2);
    
    loadEmbedResources();
}

ResourceManager::~ResourceManager()
{
    //std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_threadPool.reset(nullptr);
    m_loadedResources.clear();
}

shared_ptr<Resource> ResourceManager::findResource(const std::string &fullPath) const
{
    auto found = m_loadedResources.find(fullPath);
    if (found == m_loadedResources.end())
        return nullptr;

    return found->second;
}

shared_ptr<ResourceDecoder> ResourceManager::getDecoder(const std::string &extension) const
{
    if (extension == ".obj")
        return make_shared<DecoderOBJ>();
    if (extension == ".mtl")
        return make_shared<DecoderMTL>();
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp")
        return make_shared<DecoderTexture>();
    if (extension == ".wav")
        return make_shared<DecoderWAV>();
    if (extension == ".ogg")
        return make_shared<DecoderOGG>();
    if (extension == ".terrain")
        return make_shared<DecoderTerrain>();
    else
    {
        base::glog << "Decoder for resources of type" << extension << "doesn't exist." << base::logwarn;
        return nullptr;
    }
}

shared_ptr<Resource> ResourceManager::loadResourceFromFileSystem(const char *path, ResourceLoadingMode lm)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    // First, try to find resource with given path.
    if (auto alreadyLoadedResource = findResource(path))
        return alreadyLoadedResource;

    // Try to find resource with the full path.
    auto fullPath = g_fileSystem->fullPath(path);
    if (fullPath.empty())
    {
        base::glog << "Can't load resource. The path" << path << "doesn't exist or it's a directory." << base::logwarn;
        return nullptr;
    }

    if (auto alreadyLoadedResource = findResource(fullPath))
        return alreadyLoadedResource;

    // Create decoder for the resource.
    auto fileExtension = g_fileSystem->getFileExtension(fullPath);
    auto decoder = getDecoder(fileExtension);
    if (!decoder)
        return nullptr;

    // Create resource stub. It will not be fully valid until its completely loaded.
    auto resource = decoder->createResource();
    // Cache resource.
    m_loadedResources[fullPath] = resource;

    resource->setGUID(fullPath);

    if (m_listener)
        m_listener->onLoadFromFileSystemRequest(resource->getGUID());

    DecodeRequest request;
    request.decoder = decoder;
    request.filePath = fullPath;
    request.resource = resource;

    if (lm == ResourceLoadingMode::ASYNC)
        m_threadPool->enqueue(std::bind(&ResourceManager::doRequest, this, request));
    else if (lm == ResourceLoadingMode::IMMEDIATE)
        doRequest(request);
    else
        return nullptr;

    return resource;
}

void ResourceManager::loadResources(const std::vector<std::string> &resourcesList, ResourceLoadingMode lm)
{
    for (const auto &path : resourcesList)
        loadResourceFromFileSystem(path.c_str(), lm);
}

shared_ptr<audio::AudioBuffer> ResourceManager::createAudioBuffer(const char *audioPath)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<audio::AudioBuffer>(loadResourceFromFileSystem(audioPath, ResourceLoadingMode::IMMEDIATE));
}

shared_ptr<render::GpuProgram> ResourceManager::createGpuProgram(const char *vertexShader, const char *fragmentShader)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    // FIXME:
    // Use full path.
    std::string gpuProgramId = std::string(vertexShader) + ";" + fragmentShader;
    if (auto alreadyLoaded = findResource(gpuProgramId))
        return static_pointer_cast<render::GpuProgram>(alreadyLoaded);

    auto program = make_shared<render::GpuProgram>();
    auto vShader = render::Shader::createFromFile(vertexShader);
    auto fShader = render::Shader::createFromFile(fragmentShader);

    if (!vertexShader || !fragmentShader)
    {
        base::glog << "Can not create gpu program. Either vertex or fragment shader is invalid" << base::logwarn;
        return nullptr;
    }

    program->attachShader(vShader);
    program->attachShader(fShader);

    program->setGUID(gpuProgramId);
    program->setInitialized();
    appendResource(program);

    return program;
}

shared_ptr<render::GpuProgram> ResourceManager::createSimpleLightingGpuProgram()
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::GpuProgram>(findResource(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceManager::createFFP2DGpuProgram()
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::GpuProgram>(findResource(FFP2D_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceManager::createColoredGpuProgram()
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::GpuProgram>(findResource(COLORED_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceManager::createRttQuadProgram()
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::GpuProgram>(findResource(RTT_QUAD_PROGRAM_EMBED_PATH));
}

shared_ptr<render::GpuProgram> ResourceManager::createAmbientPassProgram()
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::GpuProgram>(findResource(AMBIENT_PASS_PROGRAM_EMBED_PATH));
}

shared_ptr<render::MeshData> ResourceManager::createMeshData(const char *meshDataPath, ResourceLoadingMode lm)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::MeshData>(loadResourceFromFileSystem(meshDataPath, lm));
}

shared_ptr<render::Texture2D> ResourceManager::createTexture(const char *imagePath, ResourceLoadingMode lm)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::Texture2D>(loadResourceFromFileSystem(imagePath, lm));
}

shared_ptr<render::Texture2D> ResourceManager::createEmptyTexture(const char *id)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (id)
    {
        if (auto alreadyLoaded = findResource(id))
            return static_pointer_cast<render::Texture2D>(alreadyLoaded);
    }

    auto texture = make_shared<render::Texture2D>();
    texture->setInitialized();
    if (id)
        texture->setGUID(id);
    appendResource(texture);

    return texture;
}

shared_ptr<render::TextureCube> ResourceManager::createCubeTexture(const char *positiveXImage, const char *negativeXImage,
                                                                   const char *positiveYImage, const char *negativeYImage,
                                                                   const char *positiveZImage, const char *negativeZImage,
                                                                   ResourceLoadingMode lm)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

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
}

shared_ptr<render::MaterialLib> ResourceManager::createMaterialLib(const char *mtlLibPath)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::MaterialLib>(loadResourceFromFileSystem(mtlLibPath, ResourceLoadingMode::IMMEDIATE));
}

void ResourceManager::appendResource(shared_ptr<Resource> resource)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto &guid = resource->getGUID();
    if (!resource->valid() || !IsGUIDValid(guid))
    {
        base::glog << "Can not append" << guid << "to resource manager because resource is not valid" << base::logwarn;
        return;
    }

    if (findResource(guid))
    {
        base::glog << "Resource" << guid << "already loaded" << base::logwarn;
        return;
    }

    m_loadedResources[guid] = resource;
}

void ResourceManager::unloadResource(const ResourceGUID &guid)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto found = m_loadedResources.find(guid);
    if (found == m_loadedResources.end() || found->second->isResident())
        return;

    m_loadedResources.erase(found);
}

void ResourceManager::unloadResource(shared_ptr<Resource> resource)
{
    unloadResource(resource->getGUID());
}

void ResourceManager::unloadUnused()
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    // Do this in such a way, because one resource can reference to an another.
    while (true)
    {
        bool somethingRemoved = false;

        auto it = m_loadedResources.cbegin();
        while (it != m_loadedResources.cend())
        {
            if (it->second.unique() && !it->second->isResident())
            {
                m_loadedResources.erase(it++);
                somethingRemoved = true;
            }
            else
                it++;
        }

        if (!somethingRemoved)
            break;
    }
}

bool ResourceManager::resourceExists(const char *path) const
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (findResource(path))
        return true;

    if (findResource(createGUIDFromPath(path)))
        return true;

    return false;
}

} }
