#pragma once

namespace df3d {

class ThreadPool;
class ResourceFileSystem;
class IResourceHolder;

class ResourceManager : NonCopyable
{
    mutable std::recursive_mutex m_lock;

    Allocator &m_allocator;
    unique_ptr<ThreadPool> m_threadPool;
    unique_ptr<ResourceFileSystem> m_fs;

    std::unordered_set<ResourceID> m_decodingResources;
    std::unordered_map<ResourceID, shared_ptr<IResourceHolder>> m_decodedResources;
    std::unordered_map<ResourceID, shared_ptr<IResourceHolder>> m_cache;

    const void* getResourceData(ResourceID resourceID) const;
    void doRequest(ResourceID resourceId, shared_ptr<IResourceHolder> loader);

public:
    ResourceManager();
    ~ResourceManager();

    void initialize();
    void shutdown();
    void poll();

    void suspend();
    void resume();

    void setDefaultFileSystem();
    void setFileSystem(unique_ptr<ResourceFileSystem> fs);

    void loadResource(ResourceID resourceId);
    void unloadResource(ResourceID resourceId);
    void loadPackage(const std::string &packagePath);
    void unloadPackage(const std::string &packagePath);
    void flush();
    bool isLoading() const;

    ResourceFileSystem& getFS() { std::lock_guard<std::recursive_mutex> lock(m_lock); return *m_fs; }

    template<typename T>
    const T* getResource(ResourceID resourceID) const { return static_cast<const T*>(getResourceData(resourceID)); }
};

}
