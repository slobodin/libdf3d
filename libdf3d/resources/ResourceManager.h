#pragma once

#include <resources/Resource.h>

FWD_MODULE_CLASS(base, ThreadPool)
FWD_MODULE_CLASS(base, Controller)

namespace df3d { namespace resources {

class ResourceDecoder;
class FileDataSource;

/**
 * Manages simple resources, like images, shaders, meshes, etc.
 */
class DF3D_DLL ResourceManager : boost::noncopyable
{
    friend class base::Controller;
public:
    enum class LoadMode
    {
        IMMEDIATE,    /**< Load resource in current thread. */
        ASYNC         /**< Load in separate thread. */
    };

private:
    struct DecodeRequest
    {
        shared_ptr<ResourceDecoder> decoder;
        std::string filePath;
        shared_ptr<Resource> resource;
    };
    void doRequest(DecodeRequest req);

    unique_ptr<base::ThreadPool> m_threadPool;
    mutable std::recursive_mutex m_lock;

    std::unordered_map<ResourceGUID, shared_ptr<Resource>> m_loadedResources;

    ResourceManager();
    ~ResourceManager();

    shared_ptr<Resource> findResource(const std::string &fullPath) const;
    shared_ptr<ResourceDecoder> getDecoder(const std::string &extension) const;

public:
    template<typename T>
    shared_ptr<T> getResource(const char *path, LoadMode lm = LoadMode::IMMEDIATE);

    shared_ptr<Resource> loadResource(const char *path, LoadMode lm = LoadMode::IMMEDIATE);
    void loadResources(const std::vector<std::string> &resourcesList, LoadMode lm = LoadMode::IMMEDIATE);

    void appendResource(shared_ptr<Resource> resource);

    void unloadResource(const ResourceGUID &guid);
    void unloadResource(shared_ptr<Resource> resource);
    void unloadUnused();

    bool isResourceExists(const char *path) const;
};

template<typename T>
shared_ptr<T> ResourceManager::getResource(const char *path, LoadMode lm)
{
    auto resFound = loadResource(path, lm);

    if (resFound && std::dynamic_pointer_cast<T>(resFound))
        return std::dynamic_pointer_cast<T>(resFound);
    else
        return nullptr;
}

} }