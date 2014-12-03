#include "df3d_pch.h"
#include "ResourceManager.h"

#include <base/Controller.h>
#include <base/ThreadPool.h>
#include "Resource.h"
#include "FileSystem.h"
#include "FileDataSource.h"
#include "decoders/DecoderOBJ.h"
#include "decoders/DecoderMTL.h"
#include "decoders/DecoderImage.h"
#include "decoders/DecoderTerrain.h"
#include "decoders/DecoderWAV.h"
#include "decoders/DecoderOGG.h"

namespace df3d { namespace resources {

void ResourceManager::doRequest(DecodeRequest req)
{
    // If 1 thread, need to be locked.
    //std::lock_guard<std::recursive_mutex> lock(m_lock);

    //base::glog << "Start load" << req.fileSource->getPath() << base::logmess;

    auto fileSource = g_fileSystem->openFile(req.filePath.c_str());

    bool decodeResult = req.decoder->decodeResource(fileSource, req.resource);
    if (decodeResult)
        req.resource->setInitialized(req.resource->init());
    else
        req.resource->setInitialized(false);

    //base::glog << "Done load" << req.fileSource->getPath() << "with result" << decodeResult << base::logmess;
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
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
        return make_shared<DecoderImage>();
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

bool ResourceManager::init()
{
    m_threadPool = make_unique<base::ThreadPool>(2);
    // TODO:
    // Add embedded resources.

    return true;
}

void ResourceManager::shutdown()
{
    //std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_threadPool.reset(nullptr);
    m_loadedResources.clear();
}

shared_ptr<Resource> ResourceManager::loadResource(const char *path, LoadMode lm)
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

    DecodeRequest request;
    request.decoder = decoder;
    request.filePath = fullPath;
    request.resource = resource;

    if (lm == LoadMode::ASYNC)
        m_threadPool->enqueue(std::bind(&ResourceManager::doRequest, this, request));
    else if (lm == LoadMode::IMMEDIATE)
        doRequest(request);
    else
        return nullptr;

    return resource;
}

void ResourceManager::loadResources(const std::vector<std::string> &resourcesList, ResourceManager::LoadMode lm)
{
    for (const auto &path : resourcesList)
        loadResource(path.c_str(), lm);
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

bool ResourceManager::isResourceExists(const char *path) const
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (findResource(path))
        return true;

    if (findResource(createGUIDFromPath(path)))
        return true;

    return false;
}

} }
