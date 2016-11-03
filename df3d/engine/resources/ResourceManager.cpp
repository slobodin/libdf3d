#include "ResourceManager.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/lib/ThreadPool.h>
#include <df3d/lib/Utils.h>
#include <df3d/lib/JsonUtils.h>
#include "ResourceFileSystem.h"
#include "GpuProgramResource.h"
#include "MaterialResource.h"
#include "MeshResource.h"
#include "ParticleSystemResource.h"
#include "TextureResource.h"
#include "EntityResource.h"
#include "IResourceHolder.h"

namespace df3d {

class ResourceLoader
{
    std::recursive_mutex m_lock;
    unique_ptr<ThreadPool> m_threadPool;
    std::unordered_set<ResourceID> m_decoding;
    std::unordered_map<ResourceID, shared_ptr<IResourceHolder>> m_decoded;
    ResourceManager &m_rmgr;
    Allocator &m_allocator;

public:
    ResourceLoader(ResourceManager &rmgr, Allocator &allocator) 
        : m_threadPool(new ThreadPool(2)),
        m_rmgr(rmgr),
        m_allocator(allocator)
    {

    }
    ~ResourceLoader()
    {
        m_threadPool.reset();
        DF3D_ASSERT(m_decoding.empty() && m_decoded.empty());
    }

    void load(ResourceID resourceId)
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);

        if (utils::contains_key(m_decoding, resourceId)
            || utils::contains_key(m_decoded, resourceId))
            return;

        shared_ptr<IResourceHolder> resourceHolder;

        const auto ext = FileSystemHelpers::getFileExtension(resourceId);
        if (ext == ".texture")
            resourceHolder = make_shared<TextureHolder>();
        else if (ext == ".shader")
            resourceHolder = make_shared<GpuProgramHolder>();
        else if (ext == ".mtl")
            resourceHolder = make_shared<MaterialLibHolder>();
        else if (ext == ".mesh")
            resourceHolder = make_shared<MeshHolder>();
        else if (ext == ".vfx")
            resourceHolder = make_shared<ParticleSystemHolder>();
        else if (ext == ".entity")
            resourceHolder = make_shared<EntityHolder>();
        else
            DF3D_FATAL("Failed to load resource: unknown resource type %s", ext.c_str());

        m_decoding.insert(resourceId);

        m_threadPool->enqueue([this, resourceId, resourceHolder]()
        {
            auto &fs = m_rmgr.getFS();
            if (auto dataSource = fs.open(resourceId.c_str()))
            {
                if (resourceHolder->decodeStartup(*dataSource, m_allocator))
                {
                    std::lock_guard<std::recursive_mutex> lock(m_lock);
                    m_decoded[resourceId] = resourceHolder;
                }
                else
                {
                    DFLOG_WARN("Resource '%s' decode failed!", resourceId.c_str());
                }

                fs.close(dataSource);
            }
            else
                DFLOG_WARN("Decode request failed. File not found.");

            {
                std::lock_guard<std::recursive_mutex> lock(m_lock);
                DF3D_VERIFY(m_decoding.erase(resourceId) == 1);
            }
        });
    }

    std::unordered_map<ResourceID, shared_ptr<IResourceHolder>>& getDecoded()
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);
        return m_decoded;
    }

    bool isLoading() const { return m_threadPool->getJobsCount() > 0; }

    void suspend()
    {
        m_threadPool->suspend();
    }

    void resume()
    {
        m_threadPool->resume();
    }
};

const void* ResourceManager::getResourceData(ResourceID resourceID)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto found = m_cache.find(resourceID);
    if (found == m_cache.end())
    {
        auto &decoded = m_loader->getDecoded();
        auto foundDecoded = decoded.find(resourceID);
        if (foundDecoded != decoded.end())
        {
            createResource(resourceID, foundDecoded->second);
            return getResourceData(resourceID);
        }
        else
            DFLOG_WARN("Resource %s lookup failed", resourceID.c_str());
    }
    else
        return found->second->getResource();

    return nullptr;
}

void ResourceManager::createResource(ResourceID resourceId, shared_ptr<IResourceHolder> loader)
{
    if (loader->createResource(m_allocator))
    {
        DF3D_ASSERT(!utils::contains_key(m_cache, resourceId));

        m_cache[resourceId] = loader;
    }
    else
    {
        DFLOG_WARN("Resource '%s' creation failed!", resourceId.c_str());
    }
    loader->decodeCleanup(m_allocator);

    DF3D_VERIFY(m_loader->getDecoded().erase(resourceId) == 1);
}

ResourceManager::ResourceManager()
    : m_allocator(MemoryManager::allocDefault())
{

}

ResourceManager::~ResourceManager()
{

}


void ResourceManager::initialize()
{
    m_loader = make_unique<ResourceLoader>(*this, m_allocator);
    setDefaultFileSystem();
}

void ResourceManager::shutdown()
{
    m_loader.reset();
    m_fs.reset();
    DF3D_ASSERT(m_cache.empty());
}

void ResourceManager::suspend()
{
    DFLOG_DEBUG("Suspending resource manager");
    m_loader->suspend();
}

void ResourceManager::resume()
{
    DFLOG_GAME("Resuming resource manager");
    m_loader->resume();
}

void ResourceManager::setDefaultFileSystem()
{
    m_fs = CreateDefaultResourceFileSystem();
}

void ResourceManager::setFileSystem(unique_ptr<ResourceFileSystem> fs)
{
    m_fs = std::move(fs);
}

void ResourceManager::loadResource(ResourceID resourceId)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (utils::contains_key(m_cache, resourceId))
        return;

    m_loader->load(resourceId);
}

void ResourceManager::unloadResource(ResourceID resourceId)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    flush();

    auto found = m_cache.find(resourceId);
    if (found != m_cache.end())
    {
        found->second->destroyResource(m_allocator);
        m_cache.erase(found);
    }
    else
        DFLOG_WARN("Can't unload resource '%s', resource is not loaded", resourceId.c_str());
}

void ResourceManager::unloadAll()
{
    flush();
    while (!m_cache.empty())
        unloadResource(m_cache.begin()->first);
}

void ResourceManager::flush()
{
    while(isLoading()) { }

    auto &decoded = m_loader->getDecoded();
    while (!decoded.empty())
        createResource(decoded.begin()->first, decoded.begin()->second);
}

bool ResourceManager::isLoading() const
{
    return m_loader->isLoading();
}

}
