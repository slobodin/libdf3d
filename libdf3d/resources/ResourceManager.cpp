#include "ResourceManager.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/base/TimeManager.h>
#include <libdf3d/utils/ThreadPool.h>
#include <libdf3d/utils/Utils.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/io/FileDataSource.h>
#include "Resource.h"
#include "ResourceFactory.h"

namespace df3d {

void ResourceManager::doRequest(DecodeRequest req)
{
    DFLOG_DEBUG("ASYNC decoding %s", req.resource->getFilePath().c_str());

    if (auto source = svc().fileSystem().openFile(req.resourcePath))
    {
        req.result = req.loader->decode(source);
        if (!req.result)
            DFLOG_WARN("ASYNC decoding failed");
    }
    else
        DFLOG_WARN("Failed to ASYNC decode a resource. Failed to open file");

    m_decodedResources.push(req);
}

shared_ptr<Resource> ResourceManager::findResource(const std::string &fullPath) const
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto found = m_loadedResources.find(fullPath);
    if (found == m_loadedResources.end())
        return nullptr;

    return found->second;
}

shared_ptr<Resource> ResourceManager::loadManual(ManualResourceLoader &loader)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto resource = shared_ptr<Resource>(loader.load());
    if (!resource)
    {
        DFLOG_WARN("Failed to manual load a resource");
        return nullptr;
    }

    resource->m_initialized = true;

    // FIXME: maybe check in cache and return existing instead?
    DF3D_ASSERT_MESS(!isResourceExist(resource->getGUID()), "resource already exists");

    m_loadedResources[resource->getGUID()] = resource;

    DFLOG_DEBUG("Manually loaded a resource");

    return resource;
}

shared_ptr<Resource> ResourceManager::loadFromFS(const std::string &path, shared_ptr<FSResourceLoader> loader)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    // First, try to find resource with given path.
    if (auto alreadyLoadedResource = findResource(path))
        return alreadyLoadedResource;

    // Try to find resource with the full path.
    auto guid = CreateGUIDFromPath(path);
    if (!IsGUIDValid(guid))
    {
        DFLOG_WARN("Can't load resource. The path %s doesn't exist or it's a directory.", path.c_str());
        return nullptr;
    }

    if (auto alreadyLoadedResource = findResource(guid))
        return alreadyLoadedResource;

    // Create resource dummy. It will not be fully valid until its completely loaded.
    auto resource = shared_ptr<Resource>(loader->createDummy());
    resource->setGUID(guid);
    // Cache the resource.
    m_loadedResources[resource->getGUID()] = resource;

    for (auto listener : m_listeners)
        listener->onLoadFromFileSystemRequest(resource->getGUID());

    DecodeRequest req;
    req.loader = loader;
    req.resource = resource;
    req.resourcePath = guid;

    if (loader->loadingMode == ResourceLoadingMode::ASYNC)
        m_threadPool->enqueue(std::bind(&ResourceManager::doRequest, this, req));
    else
    {
        DFLOG_DEBUG("Decoding %s", req.resource->getFilePath().c_str());

        req.result = loader->decode(svc().fileSystem().openFile(req.resourcePath));
        if (req.result)
        {
            loader->onDecoded(resource.get());
            resource->m_initialized = true;
        }
        else
        {
            DFLOG_WARN("Failed to decode a resource");
        }

        for (auto listener : m_listeners)
            listener->onLoadFromFileSystemRequestComplete(resource->getGUID());
    }

    return resource;
}

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::initialize()
{
    m_factory = make_unique<ResourceFactory>(this);

    m_threadPool = make_unique<utils::ThreadPool>(2);
}

void ResourceManager::shutdown()
{
    //std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_threadPool.reset(nullptr);
    m_loadedResources.clear();
    m_decodedResources.clear();
}

void ResourceManager::poll()
{
    while (!m_decodedResources.empty())
    {
        auto request = m_decodedResources.pop();
        if (request.result)
        {
            request.loader->onDecoded(request.resource.get());
            request.resource->m_initialized = true;
        }

        // Push into timemgr queue in order to invoke this when client is updated.
        for (auto listener : m_listeners)
        {
            ResourceGUID rGuid = request.resource->getGUID();
            svc().systemTimeManager().enqueueForNextUpdate([listener, rGuid]() {
                listener->onLoadFromFileSystemRequestComplete(rGuid);
            });
        }
    }
}

void ResourceManager::suspend()
{
    DFLOG_DEBUG("Suspending resource manager");
    m_threadPool->suspend();
}

void ResourceManager::resume()
{
    DFLOG_GAME("Resuming resource manager");
    m_threadPool->resume();
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

void ResourceManager::clear()
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_threadPool = make_unique<utils::ThreadPool>(2);
    m_decodedResources.clear();

    unloadUnused();
}

bool ResourceManager::isResourceExist(const ResourceGUID &guid) const
{
    return findResource(guid) != nullptr;
}

bool ResourceManager::isResourceLoaded(const ResourceGUID &guid) const
{
    auto res = findResource(guid);
    return res && res->isInitialized();
}

void ResourceManager::addListener(Listener *listener)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (utils::contains(m_listeners, listener))
    {
        DFLOG_WARN("Trying to add duplicate ResourceManager listener");
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
        DFLOG_WARN("ResourceManager::removeListener failed: listener doesn't exist");
}

}
