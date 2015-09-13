#include "df3d_pch.h"
#include "ResourceManager.h"

#include <base/Service.h>
#include <base/ThreadPool.h>
#include <utils/Utils.h>
#include "Resource.h"
#include "FileDataSource.h"

namespace df3d { namespace resources {

void ResourceManager::loadEmbedResources()
{

}

//void ResourceManager::doRequest(DecodeRequest req)
//{
    // If 1 thread, need to be locked.
    //std::lock_guard<std::recursive_mutex> lock(m_lock);

    //base::glog << "Start load" << req.fileSource->getPath() << base::logmess;

    //auto fileSource = gsvc().filesystem.openFile(req.filePath);

    //bool decodeResult = req.decoder->decodeResource(fileSource, req.resource);

    // TODO:
    // Enqueue this into engine thread.

    //req.resource->onDecoded(decodeResult);

    //{
    //    std::lock_guard<std::recursive_mutex> lock(m_lock);
    //    for (auto listener : m_listeners)
    //        listener->onLoadFromFileSystemRequestComplete(req.resource->getGUID());
    //}

    //base::glog << "Done load" << req.fileSource->getPath() << "with result" << decodeResult << base::logmess;
//}

shared_ptr<Resource> ResourceManager::findResource(const std::string &fullPath) const
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto found = m_loadedResources.find(fullPath);
    if (found == m_loadedResources.end())
        return nullptr;

    return found->second;
}

shared_ptr<Resource> ResourceManager::loadManual(shared_ptr<ManualResourceLoader> loader)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto resource = shared_ptr<Resource>(loader->load());

    // FIXME: maybe check in cache and return existing instead?
#ifdef _DEBUG
    assert(!isResourceExist(resource->getGUID()));
#endif

    m_loadedResources[resource->getGUID()] = resource;

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
        base::glog << "Can't load resource. The path" << path << "doesn't exist or it's a directory." << base::logwarn;
        return nullptr;
    }

    if (auto alreadyLoadedResource = findResource(guid))
        return alreadyLoadedResource;

    // Create resource dummy. It will not be fully valid until its completely loaded.
    auto resource = shared_ptr<Resource>(loader->createDummy());
    resource->setGUID(guid);
    // Cache the resource.
    m_loadedResources[resource->getGUID()] = resource;

    // TODO_REFACTO: async calls.

    for (auto listener : m_listeners)
        listener->onLoadFromFileSystemRequest(resource->getGUID());

    loader->decode(gsvc().filesystem.openFile(guid));
    loader->onDecoded(resource.get());

    for (auto listener : m_listeners)
        listener->onLoadFromFileSystemRequestComplete(resource->getGUID());

    return resource;
}

ResourceManager::ResourceManager()
{
    m_threadPool = make_unique<base::ThreadPool>(2);
    m_factory = make_unique<ResourceFactory>(this);
}

ResourceManager::~ResourceManager()
{
    //std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_threadPool.reset(nullptr);
    m_loadedResources.clear();
}

//shared_ptr<Resource> ResourceManager::loadResourceFromFileSystem(const std::string &path, ResourceLoadingMode lm)
//{
//    if (lm == ResourceLoadingMode::ASYNC)
//        m_threadPool->enqueue(std::bind(&ResourceManager::doRequest, this, request));
//    else if (lm == ResourceLoadingMode::IMMEDIATE)
//        doRequest(request);
//    else
//        return nullptr;
//
//    return resource;
//}

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
