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

static shared_ptr<IResourceHolder> CreateResourceHolder(const char *resourcePath)
{
    shared_ptr<IResourceHolder> resourceHolder;

    if (FileSystemHelpers::compareExtension(resourcePath, ".texture"))
        resourceHolder = make_shared<TextureHolder>();
    else if (FileSystemHelpers::compareExtension(resourcePath, ".shader"))
        resourceHolder = make_shared<GpuProgramHolder>();
    else if (FileSystemHelpers::compareExtension(resourcePath, ".mtl"))
        resourceHolder = make_shared<MaterialLibHolder>();
    else if (FileSystemHelpers::compareExtension(resourcePath, ".mesh"))
        resourceHolder = make_shared<MeshHolder>();
    else if (FileSystemHelpers::compareExtension(resourcePath, ".vfx"))
        resourceHolder = make_shared<ParticleSystemHolder>();
    else if (FileSystemHelpers::compareExtension(resourcePath, ".entity"))
        resourceHolder = make_shared<EntityHolder>(false);
    else if (FileSystemHelpers::compareExtension(resourcePath, ".world"))
        resourceHolder = make_shared<EntityHolder>(true);
    else
        DF3D_ASSERT_MESS(false, "Failed to create resource decoder: unknown resource type '%s'", resourcePath);

    return resourceHolder;
}

struct LoadingState
{
    ThreadPool pool;
    std::unordered_map<Id, shared_ptr<IResourceHolder>> decoded;
    std::unordered_map<Id, std::unordered_set<std::string>> dependencies;

    LoadingState(size_t workers) : pool(workers) { }
    ~LoadingState() { DF3D_ASSERT(decoded.empty()); }
};

const void* ResourceManager::getResourceData(Id resourceID)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto found = m_cache.find(resourceID);
    if (found != m_cache.end())
    {
        DF3D_ASSERT(found->second.valid);
        found->second.used = true;
        return found->second.holder->getResource();
    }

    DFLOG_WARN("Resource %s lookup failed", resourceID.toString().c_str());
    return nullptr;
}

void ResourceManager::listDependencies(const ResourcePackage &input, ResourcePackage &output, LoadingState *loadingState)
{
    for (const auto &resourcePath : input)
    {
        if (auto holder = CreateResourceHolder(resourcePath.c_str()))
        {
            if (auto dataSource = getFS().open(resourcePath.c_str()))
            {
                output.push_back(resourcePath);

                ResourcePackage tmp;
                holder->listDependencies(*dataSource, tmp);
                getFS().close(dataSource);

                if (loadingState)
                {
                    Id resourceId(resourcePath.c_str());
                    loadingState->dependencies[resourceId].insert(tmp.begin(), tmp.end());
                }

                listDependencies(tmp, output, loadingState);
            }
        }
    }
}

void ResourceManager::unloadResource(Id resource)
{
    auto found = m_cache.find(resource);
    if (found != m_cache.end())
    {
        if (--found->second.refCount == 0)
        {
            found->second.holder->destroyResource(m_allocator);
            m_cache.erase(found);
        }
    }
    else
        DF3D_ASSERT_MESS(false, "Failed to unload resoure. Resource '%s' not found", resource.toString().c_str());
}

ResourceManager::ResourceManager()
    : m_allocator(MemoryManager::allocDefault())
{
    int thr = std::thread::hardware_concurrency();
    m_maxThreadPoolWorkers = df3d::utils::clamp(thr, 1, 2);
}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::initialize()
{
    setDefaultFileSystem();
}

void ResourceManager::shutdown()
{
    m_loadingState.reset();
    m_fs.reset();
    DF3D_ASSERT(m_cache.empty());
}

void ResourceManager::poll()
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    if (m_loadingState)
    {
        for (auto it = m_loadingState->decoded.begin(); it != m_loadingState->decoded.end(); )
        {
            auto holder = it->second;
            auto resourceId = it->first;

            auto &deps = m_loadingState->dependencies[resourceId];
            bool canCreate = true;
            for (const auto &depPath : deps)
            {
                auto found = m_cache.find(Id(depPath.c_str()));
                if (!(found != m_cache.end() && found->second.valid))
                {
                    canCreate = false;
                    break;
                }
            }

            if (!canCreate)
            {
                ++it;
                continue;
            }

            if (holder->createResource(m_allocator))
            {
                m_cache[resourceId].valid = true;
            }
            else
            {
                DFLOG_WARN("Resource '%s' creation failed!", resourceId.toString().c_str());
                m_cache.erase(resourceId);
            }
            holder->decodeCleanup(m_allocator);
            it = m_loadingState->decoded.erase(it);
        }

        if (m_loadingState->pool.getCurrentJobsCount() == 0 && m_loadingState->decoded.empty())
            m_loadingState.reset();
    }
}

void ResourceManager::suspend()
{
    DFLOG_DEBUG("ResourceManager start suspend");
    if (m_loadingState)
        m_loadingState->pool.suspend();

    DFLOG_DEBUG("ResourceManager SUSPENDED");
}

void ResourceManager::resume()
{
    DFLOG_DEBUG("ResourceManager::resume");
    if (m_loadingState)
        m_loadingState->pool.resume();
}

void ResourceManager::setDefaultFileSystem()
{
    m_fs = CreateDefaultResourceFileSystem();
}

void ResourceManager::setFileSystem(unique_ptr<ResourceFileSystem> fs)
{
    m_fs = std::move(fs);
}

void ResourceManager::loadPackageAsync(const ResourcePackage &resources)
{
    if (resources.empty())
        return;

    if (isLoading())
    {
        DF3D_ASSERT_MESS(false, "Can not load packages simultaneously!");
        return;
    }

    m_loadingState = make_unique<LoadingState>(m_maxThreadPoolWorkers);

    ResourcePackage resourcesToLoad;
    listDependencies(resources, resourcesToLoad, m_loadingState.get());

    for (const auto &resourcePath : resourcesToLoad)
    {
        auto holder = CreateResourceHolder(resourcePath.c_str());
        if (!holder)
            continue;

        m_loadingState->pool.enqueue([this, resourcePath, holder]()
        {
            Id resourceId(resourcePath.c_str());

            {
                std::lock_guard<std::recursive_mutex> lock(m_lock);

                auto found = m_cache.find(resourceId);
                if (found == m_cache.end())
                {
                    Entry entry;
                    entry.refCount = 1;
                    entry.valid = false;
                    entry.holder = holder;

#ifdef _DEBUG
                    entry.resourcePath = resourcePath;
#endif

                    m_cache[resourceId] = entry;
                }
                else
                {
                    found->second.refCount++;
                    return;
                }
            }

            auto &fs = getFS();

            auto dataSource = fs.open(resourcePath.c_str());
            bool decodeResult = holder->decodeStartup(*dataSource, m_allocator);

            {
                std::lock_guard<std::recursive_mutex> lock(m_lock);
                if (decodeResult)
                {
                    DF3D_ASSERT(!utils::contains_key(m_loadingState->decoded, resourceId));
                    m_loadingState->decoded[resourceId] = holder;
                }
                else
                {
                    DFLOG_WARN("Resource '%s' decode failed!", resourcePath.c_str());
                    DF3D_VERIFY(m_cache.erase(resourceId) == 1);
                }
            }

            fs.close(dataSource);
        });
    }
}

void ResourceManager::unloadPackage(const ResourcePackage &resources)
{
    if (resources.empty())
        return;
    flush();

    std::vector<std::string> resourcesToUnLoad;
    listDependencies(resources, resourcesToUnLoad, nullptr);

    for (const auto &resourcePath : resourcesToUnLoad)
        unloadResource(Id(resourcePath.c_str()));
}

bool ResourceManager::isLoading() const
{
    return m_loadingState && 
        ((m_loadingState->pool.getCurrentJobsCount() > 0) ||
        (!m_loadingState->decoded.empty()));
}

void ResourceManager::flush()
{
    while (isLoading()) { poll(); }
}

void ResourceManager::printUnused()
{
    DFLOG_DEBUG("Unused resources:");
    //for (const auto &entry : m_cache)
    //{
    //    if (!entry.second.used)
    //        DFLOG_DEBUG("%s", entry.first.c_str());
    //}

    DFLOG_DEBUG("---");
}

std::vector<std::string> ResourceManager::getLoadedResourcesIds() const
{
#ifdef _DEBUG
    if (isLoading())
    {
        DF3D_ASSERT(false);
        return {};
    }

    std::vector<std::string> result;
    for (const auto &kv : m_cache)
        result.push_back(kv.second.resourcePath);
    return result;
#else
    DF3D_ASSERT(false);
    return {};
#endif
}

}
