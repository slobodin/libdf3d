#pragma once

FWD_MODULE_CLASS(base, ThreadPool)
FWD_MODULE_CLASS(base, EngineController)

namespace df3d { namespace resources {

class Resource;
class ResourceDecoder;
class ResourceFactory;
class FileDataSource;

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
    friend class ResourceManager;

    struct DecodeRequest
    {
        shared_ptr<ResourceDecoder> decoder;
        std::string filePath;
        shared_ptr<Resource> resource;
    };

    unique_ptr<base::ThreadPool> m_threadPool;
    mutable std::recursive_mutex m_lock;

    std::unordered_map<ResourceGUID, shared_ptr<Resource>> m_loadedResources;
    
    std::vector<Listener*> m_listeners;

    unique_ptr<ResourceFactory> m_factory;

    void loadEmbedResources();
    void doRequest(DecodeRequest req);
    void appendResource(shared_ptr<Resource> resource);

    ResourceManager();
    ~ResourceManager();

    shared_ptr<Resource> findResource(const std::string &guid) const;
    shared_ptr<Resource> loadResourceFromFileSystem(const std::string &path, ResourceLoadingMode lm);

public:
    //! All resources creation is going through this factory.
    ResourceFactory& getFactory() { return *m_factory; }

    void unloadResource(const ResourceGUID &guid);
    void unloadResource(shared_ptr<Resource> resource);
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
