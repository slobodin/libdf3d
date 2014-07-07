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
    enum LoadMode
    {
        LOAD_MODE_IMMEDIATE,    /**< Load resource in current thread. */
        LOAD_MODE_ASYNC         /**< Load in separate thread. */
    };

private:
    struct DecodeRequest
    {
        shared_ptr<ResourceDecoder> decoder;
        std::string filePath;
        shared_ptr<Resource> resource;
    };
    void doRequest(DecodeRequest req);

    scoped_ptr<base::ThreadPool> m_threadPool;
    mutable std::recursive_mutex m_lock;

    std::unordered_map<ResourceGUID, shared_ptr<Resource>> m_loadedResources;

    ResourceManager();
    ~ResourceManager();

    shared_ptr<Resource> findResource(const std::string &fullPath) const;
    shared_ptr<ResourceDecoder> getDecoder(const std::string &extension) const;

    // These are only for controller.
    bool init();
    void shutdown();

public:
    template<typename T>
    shared_ptr<T> getResource(const char *path, LoadMode lm = LOAD_MODE_IMMEDIATE);

    shared_ptr<Resource> loadResource(const char *path, LoadMode lm = LOAD_MODE_IMMEDIATE);
    void loadResource(shared_ptr<Resource> resource);
    void unloadResource(const ResourceGUID &guid);
    void unloadResource(shared_ptr<Resource> resource);

    void unloadUnused();

    bool isResourceExists(const char *path) const;
};

template<typename T>
shared_ptr<T> ResourceManager::getResource(const char *path, LoadMode lm)
{
    auto resFound = loadResource(path, lm);

    if (resFound && boost::dynamic_pointer_cast<T>(resFound))
        return boost::dynamic_pointer_cast<T>(resFound);
    else
        return nullptr;
}

} }