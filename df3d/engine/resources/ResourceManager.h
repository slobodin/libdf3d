#pragma once

#include <df3d/lib/containers/ConcurrentQueue.h>

namespace df3d {

class Resource;
class ResourceFactory;
class ManualResourceLoader;
class FSResourceLoader;
class ThreadPool;

class DF3D_DLL ResourceManager : NonCopyable
{
public:
    class Listener
    {
    public:
        virtual ~Listener() { }

        virtual void onLoadFromFileSystemRequest(ResourceGUID resourceId) = 0;
        virtual void onLoadFromFileSystemRequestComplete(ResourceGUID resourceId) = 0;
    };

private:
    friend class ResourceFactory;

    struct DecodeRequest
    {
        std::string resourcePath;
        shared_ptr<Resource> resource;
        shared_ptr<FSResourceLoader> loader;

        bool result = false;
    };

    // Thread pool for resources decoding.
    unique_ptr<ThreadPool> m_threadPool;
    // Resources for which should call onDecoded in the main thread.
    ConcurrentQueue<DecodeRequest> m_decodedResources;
    mutable std::recursive_mutex m_lock;

    std::unordered_map<ResourceGUID, shared_ptr<Resource>> m_loadedResources;
    
    std::vector<Listener*> m_listeners;

    unique_ptr<ResourceFactory> m_factory;

    void doRequest(DecodeRequest req);

    shared_ptr<Resource> findResource(const std::string &guid) const;
    shared_ptr<Resource> loadManual(ManualResourceLoader &loader);
    shared_ptr<Resource> loadFromFS(const std::string &path, shared_ptr<FSResourceLoader> loader);

public:
    ResourceManager();
    ~ResourceManager();

    void initialize();
    void shutdown();
    void poll();

    void suspend();
    void resume();

    //! All resources creation is going through this factory.
    ResourceFactory& getFactory() { return *m_factory; }

    //! Unloads a resource with given guid.
    void unloadResource(const ResourceGUID &guid);
    //! Unloads a given resource.
    void unloadResource(shared_ptr<Resource> resource);
    //! Unloads all resources with refcount equals to 1.
    void unloadUnused();

    void clear();

    //! Checks whether or not resource is present in the resource manager cache.
    /*!
     * \brief NOTE: Resource may not be decoded yet.
     * \param guid
     * \return
     */
    bool isResourceExist(const ResourceGUID &guid) const;
    //! Checks whether or not resource exists and loaded (i.e. successfully decoded).
    /*!
     * \param guid
     * \return
     */
    bool isResourceLoaded(const ResourceGUID &guid) const;

    size_t getLoadedResourcesCount() const { return m_loadedResources.size(); }

    void addListener(Listener *listener);
    void removeListener(Listener *listener);
};

}
