#include "df3d_pch.h"
#include "ResourceManager.h"

#include <base/SystemsMacro.h>
#include <base/ThreadPool.h>
#include <utils/Utils.h>
#include <render/Shader.h>
#include <render/GpuProgram.h>
#include "Resource.h"
#include "ResourceDecoder.h"
#include "ResourceFactory.h"
#include "FileDataSource.h"

namespace df3d { namespace resources {

const char * const SIMPLE_LIGHTING_PROGRAM_EMBED_PATH = "__embed_simple_lighting_program";
const char * const RTT_QUAD_PROGRAM_EMBED_PATH = "__embed_quad_render_program";
const char * const COLORED_PROGRAM_EMBED_PATH = "__embed_colored_program";
const char * const AMBIENT_PASS_PROGRAM_EMBED_PATH = "__embed_ambient_pass_program";

void ResourceManager::loadEmbedResources()
{
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

    getFactory().createGpuProgram(SIMPLE_LIGHTING_PROGRAM_EMBED_PATH, simple_lighting_vert, simple_lighting_frag)->setResident(true);
    getFactory().createGpuProgram(RTT_QUAD_PROGRAM_EMBED_PATH, rtt_quad_vert, rtt_quad_frag)->setResident(true);
    getFactory().createGpuProgram(COLORED_PROGRAM_EMBED_PATH, colored_vert, colored_frag)->setResident(true);
    getFactory().createGpuProgram(AMBIENT_PASS_PROGRAM_EMBED_PATH, ambient_vert, ambient_frag)->setResident(true);
}

void ResourceManager::doRequest(DecodeRequest req)
{
    // If 1 thread, need to be locked.
    //std::lock_guard<std::recursive_mutex> lock(m_lock);

    //base::glog << "Start load" << req.fileSource->getPath() << base::logmess;

    auto fileSource = g_fileSystem->openFile(req.filePath);

    bool decodeResult = req.decoder->decodeResource(fileSource, req.resource);

    // TODO:
    // Enqueue this into engine thread.

    req.resource->onDecoded(decodeResult);

    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);
        for (auto listener : m_listeners)
            listener->onLoadFromFileSystemRequestComplete(req.resource->getGUID());
    }

    //base::glog << "Done load" << req.fileSource->getPath() << "with result" << decodeResult << base::logmess;
}

void ResourceManager::appendResource(shared_ptr<Resource> resource)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    const auto &guid = resource->getGUID();
    if (!resource->isInitialized() || !IsGUIDValid(guid))
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

ResourceManager::ResourceManager()
{
    m_threadPool = make_unique<base::ThreadPool>(2);
    m_factory = make_unique<ResourceFactory>(this);

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
    auto guid = CreateGUIDFromPath(path);
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
    return res && res->isInitialized();
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
