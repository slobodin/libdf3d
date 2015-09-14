#pragma once

#include <utils/ConcurrentQueue.h>

FWD_MODULE_CLASS(base, ThreadPool)
FWD_MODULE_CLASS(base, EngineController)

namespace df3d { namespace resources {

class Resource;
class ResourceFactory;
class FileDataSource;
class ManualResourceLoader;
class FSResourceLoader;

class DF3D_DLL ResourceManager : utils::NonCopyable
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
    friend class base::EngineController;
    friend class ResourceFactory;

    struct DecodeRequest
    {
        shared_ptr<FileDataSource> source;
        shared_ptr<Resource> resource;
        shared_ptr<FSResourceLoader> loader;
    };

    // Thread pool for resources decoding.
    unique_ptr<base::ThreadPool> m_threadPool;
    // Resources for which should call onDecoded in the main thread.
    utils::ConcurrentQueue<DecodeRequest> m_decodedResources;
    mutable std::recursive_mutex m_lock;

    std::unordered_map<ResourceGUID, shared_ptr<Resource>> m_loadedResources;
    
    std::vector<Listener*> m_listeners;

    unique_ptr<ResourceFactory> m_factory;

    void loadEmbedResources();
    void doRequest(DecodeRequest req);

    shared_ptr<Resource> findResource(const std::string &guid) const;
    shared_ptr<Resource> loadManual(shared_ptr<ManualResourceLoader> loader);
    shared_ptr<Resource> loadFromFS(const std::string &path, shared_ptr<FSResourceLoader> loader);

    ResourceManager();
    ~ResourceManager();

    void poll();

public:
    //! All resources creation is going through this factory.
    ResourceFactory& getFactory() { return *m_factory; }

    //! Unloads a resource with given guid.
    void unloadResource(const ResourceGUID &guid);
    //! Unloads a given resource.
    void unloadResource(shared_ptr<Resource> resource);
    //! Unloads all resources with refcount equals to 1.
    void unloadUnused();

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

    void addListener(Listener *listener);
    void removeListener(Listener *listener);
};

} }
