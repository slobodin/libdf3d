#pragma once

namespace df3d {

class ThreadPool;
class ResourceFileSystem;
class IResourceHolder;
class ResourceLoader;

class ResourceManager : NonCopyable
{
    mutable std::recursive_mutex m_lock;
    Allocator &m_allocator;
    unique_ptr<ResourceFileSystem> m_fs;
    unique_ptr<ResourceLoader> m_loader;

    std::unordered_map<ResourceID, shared_ptr<IResourceHolder>> m_cache;

    const void* getResourceData(ResourceID resourceID);
    void createResource(ResourceID resourceId, shared_ptr<IResourceHolder> loader);

public:
    ResourceManager();
    ~ResourceManager();

    void initialize();
    void shutdown();

    void suspend();
    void resume();

    void setDefaultFileSystem();
    void setFileSystem(unique_ptr<ResourceFileSystem> fs);

    // TODO: decodeREsources
    // Create resources
    // block until loads done.
    // Remove package

    void loadResource(ResourceID resourceId);
    void unloadResource(ResourceID resourceId);
    void unloadAll();
    void flush();
    bool isLoading() const;

    ResourceFileSystem& getFS() { std::lock_guard<std::recursive_mutex> lock(m_lock); return *m_fs; }

    template<typename T>
    const T* getResource(ResourceID resourceID) { return static_cast<const T*>(getResourceData(resourceID)); }
};

}
