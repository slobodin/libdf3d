#pragma once

#include <unordered_map>
#include <df3d/lib/Id.h>
#include <df3d/Common.h>
#include <mutex>
#include <df3d/lib/NonCopyable.h>
#include <vector>
#include <string>

namespace df3d {

class ThreadPool;
class ResourceFileSystem;
class IResourceHolder;
struct LoadingState;
class Allocator;

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
        shared_ptr<IResourceHolder> holder;
        int refCount = 0;
        bool valid = false;
        bool used = false;
    };

    std::unordered_map<Id, Entry> m_cache;

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

    // Can load only 1 package at a time.
    void loadPackageAsync(const ResourcePackage &resources);
    void unloadPackage(const ResourcePackage &resources);
    bool isLoading() const;
    void flush();
    void printUnused();

    ResourceFileSystem& getFS() { std::lock_guard<std::recursive_mutex> lock(m_lock); return *m_fs; }

    template<typename T>
    const T* getResource(Id resourceID) { return static_cast<const T*>(getResourceData(resourceID)); }
};

}
