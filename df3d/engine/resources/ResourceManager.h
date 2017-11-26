#pragma once

namespace df3d {

class ThreadPool;
class ResourceFileSystem;
class IResourceHolder;
struct LoadingState;

// TODO: refactor

using ResourcePackage = std::vector<std::string>;

class ResourceManager : NonCopyable
{
    friend struct LoadingState;

    mutable std::recursive_mutex m_lock;
    Allocator &m_allocator;
    unique_ptr<ResourceFileSystem> m_fs;
    unique_ptr<LoadingState> m_loadingState;

    struct Entry
    {
#ifdef _DEBUG
        std::string resourcePath;
#endif
        shared_ptr<IResourceHolder> holder;
        int refCount = 0;
        bool valid = false;
        bool used = false;
    };

    std::unordered_map<Id, Entry> m_cache;

    size_t m_maxThreadPoolWorkers;
    bool m_lowEndDevice = false;

    const void* getResourceData(Id resourceID);
    void listDependencies(const ResourcePackage &input, ResourcePackage &output, LoadingState *loadingState);
    void unloadResource(Id resource);

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

    void setMaxThreadPoolWorkers(size_t num) { m_maxThreadPoolWorkers = num; }
    void setIsLowEndDevice(bool lowend) { m_lowEndDevice = lowend; }
    bool getIsLowEndDevice() const { return m_lowEndDevice; }

    // Can load only 1 package at a time.
    void loadPackageAsync(const ResourcePackage &resources);
    void unloadPackage(const ResourcePackage &resources);
    bool isLoading() const;
    void flush();
    void printUnused();

    ResourceFileSystem& getFS() { std::lock_guard<std::recursive_mutex> lock(m_lock); return *m_fs; }

    template<typename T>
    const T* getResource(Id resourceID) { return static_cast<const T*>(getResourceData(resourceID)); }

    bool isLoaded(Id resourceID) { return getResourceData(resourceID); }

    size_t getLoadedResourcesCount() const { return m_cache.size(); }
    std::vector<std::string> getLoadedResourcesIds() const;
};

}
