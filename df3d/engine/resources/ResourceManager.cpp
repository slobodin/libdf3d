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
#include "AudioResource.h"
#include "IResourceHolder.h"

namespace df3d {

static shared_ptr<IResourceHolder> CreateResourceHolder(const ResourceID &resourceId)
{
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
    else if (ext == ".audio")
        resourceHolder = make_shared<AudioResourceHolder>();
    else
        DF3D_ASSERT_MESS(false, "Failed to create resource decoder: unknown resource type %s", ext.c_str());

    return resourceHolder;
}

struct LoadingState
{
    ThreadPool pool;
    std::unordered_map<ResourceID, shared_ptr<IResourceHolder>> decoded;
    std::unordered_map<ResourceID, std::unordered_set<ResourceID>> dependencies;

    LoadingState() : pool(std::max(1u, std::thread::hardware_concurrency())) { }
    ~LoadingState() { DF3D_ASSERT(decoded.empty()); }
};

const void* ResourceManager::getResourceData(const ResourceID &resourceID)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto found = m_cache.find(resourceID);
    if (found != m_cache.end())
    {
        DF3D_ASSERT(found->second.valid);
        return found->second.holder->getResource();
    }

    DFLOG_WARN("Resource %s lookup failed", resourceID.c_str());
    return nullptr;
}

void ResourceManager::listDependencies(const ResourcePackage &input, ResourcePackage &output, LoadingState *loadingState)
{
    for (const auto &resourceId : input)
    {
        if (auto holder = CreateResourceHolder(resourceId))
        {
            if (auto dataSource = getFS().open(resourceId.c_str()))
            {
                output.push_back(resourceId);

                ResourcePackage tmp;
                holder->listDependencies(*dataSource, tmp);
                getFS().close(dataSource);

                if (loadingState)
                    loadingState->dependencies[resourceId].insert(tmp.begin(), tmp.end());

                listDependencies(tmp, output, loadingState);
            }
        }
    }
}

void ResourceManager::unloadResource(const ResourceID &resource)
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
        DF3D_ASSERT_MESS(false, "Failed to unload resoure. Resource '%s' not found", resource.c_str());
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

            auto &deps = m_loadingState->dependencies;
            bool canCreate = true;
            for (const auto &dep : deps[resourceId])
            {
                auto found = m_cache.find(dep);
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
                DFLOG_WARN("Resource '%s' creation failed!", resourceId.c_str());
                m_cache.erase(resourceId);
            }
            holder->decodeCleanup(m_allocator);
            it = m_loadingState->decoded.erase(it);
        }

        if (m_loadingState->pool.getCurrentJobsCount() == 0 && m_loadingState->decoded.empty())
            m_loadingState.reset();
    }
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
    if (isLoading())
    {
        DF3D_ASSERT_MESS(false, "Can not load packages simultaneously!");
        return;
    }

    m_loadingState = make_unique<LoadingState>();

    std::vector<ResourceID> resourcesToLoad;
    listDependencies(resources, resourcesToLoad, m_loadingState.get());

    for (const auto &resourceId : resourcesToLoad)
    {
        auto holder = CreateResourceHolder(resourceId);
        if (!holder)
            continue;

        m_loadingState->pool.enqueue([this, resourceId, holder]()
        {
            {
                std::lock_guard<std::recursive_mutex> lock(m_lock);

                auto found = m_cache.find(resourceId);
                if (found == m_cache.end())
                {
                    Entry entry;
                    entry.refCount = 1;
                    entry.valid = false;
                    entry.holder = holder;

                    m_cache[resourceId] = entry;
                }
                else
                {
                    found->second.refCount++;
                    return;
                }
            }

            auto &fs = getFS();

            auto dataSource = fs.open(resourceId.c_str());
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
                    DFLOG_WARN("Resource '%s' decode failed!", resourceId.c_str());
                    DF3D_VERIFY(m_cache.erase(resourceId) == 1);
                }
            }

            fs.close(dataSource);
        });
    }
}

void ResourceManager::unloadPackage(const ResourcePackage &resources)
{
    flush();

    std::vector<ResourceID> resourcesToUnLoad;
    listDependencies(resources, resourcesToUnLoad, nullptr);

    for (const auto &resourceId : resourcesToUnLoad)
        unloadResource(resourceId);
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

}
