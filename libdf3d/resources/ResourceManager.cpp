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
#include <utils/Utils.h>
#include "Resource.h"
#include "ResourceDecoder.h"
#include "FileDataSource.h"

namespace df3d { namespace resources {

const char * const SIMPLE_LIGHTING_PROGRAM_EMBED_PATH = "__embed_simple_lighting_program";
const char * const RTT_QUAD_PROGRAM_EMBED_PATH = "__embed_quad_render_program";
const char * const COLORED_PROGRAM_EMBED_PATH = "__embed_colored_program";
const char * const AMBIENT_PASS_PROGRAM_EMBED_PATH = "__embed_ambient_pass_program";

void ResourceManager::loadEmbedResources()
{
    auto loadInternal = [&](const std::string &name, const std::string &dataVert, const std::string &dataFrag)
    {
        auto program = make_shared<render::GpuProgram>();
        auto vertexShader = render::Shader::createFromString(dataVert, render::Shader::Type::VERTEX);
        auto fragmentShader = render::Shader::createFromString(dataFrag, render::Shader::Type::FRAGMENT);

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
    loadInternal(RTT_QUAD_PROGRAM_EMBED_PATH, rtt_quad_vert, rtt_quad_frag);
    loadInternal(COLORED_PROGRAM_EMBED_PATH, colored_vert, colored_frag);
    loadInternal(AMBIENT_PASS_PROGRAM_EMBED_PATH, ambient_vert, ambient_frag);
}

void ResourceManager::doRequest(DecodeRequest req)
{
    // If 1 thread, need to be locked.
    //std::lock_guard<std::recursive_mutex> lock(m_lock);

    //base::glog << "Start load" << req.fileSource->getPath() << base::logmess;

    auto fileSource = g_fileSystem->openFile(req.filePath);

    bool decodeResult = req.decoder->decodeResource(fileSource, req.resource);
    req.resource->setInitialized(true);

    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);
        for (auto listener : m_listeners)
            listener->onLoadFromFileSystemRequestComplete(req.resource->getGUID());
    }

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

shared_ptr<Resource> ResourceManager::loadResourceFromFileSystem(const std::string &path, ResourceLoadingMode lm)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    // First, try to find resource with given path.
    if (auto alreadyLoadedResource = findResource(path))
        return alreadyLoadedResource;

    // Try to find resource with the full path.
    auto guid = createGUIDFromPath(path);
    if (!IsGUIDValid(guid))
    {
        base::glog << "Can't load resource. The path" << path << "doesn't exist or it's a directory." << base::logwarn;
        return nullptr;
    }

    if (auto alreadyLoadedResource = findResource(guid))
        return alreadyLoadedResource;

    // Create decoder for the resource.
    auto decoder = createResourceDecoder(guid);
    if (!decoder)
        return nullptr;

    // Create resource stub. It will not be fully valid until its completely loaded.
    auto resource = decoder->createResource();
    // Cache resource.
    m_loadedResources[guid] = resource;

    resource->setGUID(guid);

    for (auto listener : m_listeners)
        listener->onLoadFromFileSystemRequest(resource->getGUID());

    DecodeRequest request;
    request.decoder = decoder;
    request.filePath = guid;
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
        loadResourceFromFileSystem(path, lm);
}

shared_ptr<audio::AudioBuffer> ResourceManager::createAudioBuffer(const std::string &audioPath)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<audio::AudioBuffer>(loadResourceFromFileSystem(audioPath, ResourceLoadingMode::IMMEDIATE));
}

shared_ptr<render::GpuProgram> ResourceManager::createGpuProgram(const std::string &vertexShader, const std::string &fragmentShader)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    // FIXME:
    // Use full path.
    std::string gpuProgramId = vertexShader + ";" + fragmentShader;
    if (auto alreadyLoaded = findResource(gpuProgramId))
        return static_pointer_cast<render::GpuProgram>(alreadyLoaded);

    auto program = make_shared<render::GpuProgram>();
    auto vShader = render::Shader::createFromFile(vertexShader);
    auto fShader = render::Shader::createFromFile(fragmentShader);

    if (!vShader || !fShader)
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

shared_ptr<render::MeshData> ResourceManager::createMeshData(const std::string &meshDataPath, ResourceLoadingMode lm)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::MeshData>(loadResourceFromFileSystem(meshDataPath, lm));
}

shared_ptr<render::Texture2D> ResourceManager::createTexture(const std::string &imagePath, ResourceLoadingMode lm)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::Texture2D>(loadResourceFromFileSystem(imagePath, lm));
}

shared_ptr<render::Texture2D> ResourceManager::createEmptyTexture(const std::string &id)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (!id.empty())
    {
        if (auto alreadyLoaded = findResource(id))
            return static_pointer_cast<render::Texture2D>(alreadyLoaded);
    }

    auto texture = make_shared<render::Texture2D>();
    texture->setInitialized();
    if (!id.empty())
        texture->setGUID(id);
    appendResource(texture);

    return texture;
}

shared_ptr<render::TextureCube> ResourceManager::createCubeTexture(const std::string &positiveXImage, const std::string &negativeXImage,
                                                                   const std::string &positiveYImage, const std::string &negativeYImage,
                                                                   const std::string &positiveZImage, const std::string &negativeZImage,
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

shared_ptr<render::MaterialLib> ResourceManager::createMaterialLib(const std::string &mtlLibPath)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return static_pointer_cast<render::MaterialLib>(loadResourceFromFileSystem(mtlLibPath, ResourceLoadingMode::IMMEDIATE));
}

void ResourceManager::appendResource(shared_ptr<Resource> resource)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    const auto &guid = resource->getGUID();
    if (!resource->isValid() || !IsGUIDValid(guid))
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

    m_threadPool->clear();

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

bool ResourceManager::isResourceExist(const ResourceGUID &guid) const
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    return findResource(guid) != nullptr;
}

bool ResourceManager::isResourceLoaded(const ResourceGUID &guid) const
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto res = findResource(guid);
    return res && res->isValid();
}

void ResourceManager::addListener(Listener *listener)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (utils::contains(m_listeners, listener))
    {
        base::glog << "Trying to add duplicate ResourceManager listener" << base::logwarn;
        return;
    }

    m_listeners.push_back(listener);
}

void ResourceManager::removeListener(Listener *listener)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto found = std::find(m_listeners.begin(), m_listeners.end(), listener);
    if (found != m_listeners.end())
        m_listeners.erase(found);
    else
        base::glog << "ResourceManager::removeListener failed: listener doesn't exist" << base::logwarn;
}

} }
