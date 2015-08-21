#pragma once

FWD_MODULE_CLASS(base, ThreadPool)
FWD_MODULE_CLASS(base, EngineController)
FWD_MODULE_CLASS(audio, AudioBuffer)
FWD_MODULE_CLASS(render, Texture2D)
FWD_MODULE_CLASS(render, TextureCube)
FWD_MODULE_CLASS(render, GpuProgram)
FWD_MODULE_CLASS(render, MeshData)
FWD_MODULE_CLASS(render, MaterialLib)

namespace df3d { namespace resources {

class Resource;
class ResourceDecoder;
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

    struct DecodeRequest
    {
        shared_ptr<ResourceDecoder> decoder;
        std::string filePath;
        shared_ptr<Resource> resource;
    };

    void loadEmbedResources();
    void doRequest(DecodeRequest req);

    unique_ptr<base::ThreadPool> m_threadPool;
    mutable std::recursive_mutex m_lock;

    std::unordered_map<ResourceGUID, shared_ptr<Resource>> m_loadedResources;
    
    std::vector<Listener*> m_listeners;

    ResourceManager();
    ~ResourceManager();

    shared_ptr<Resource> findResource(const std::string &guid) const;
    shared_ptr<Resource> loadResourceFromFileSystem(const std::string &path, ResourceLoadingMode lm);

public:
    shared_ptr<audio::AudioBuffer> createAudioBuffer(const std::string &audioPath);
    shared_ptr<render::GpuProgram> createGpuProgram(const std::string &vertexShader, const std::string &fragmentShader);
    shared_ptr<render::GpuProgram> createSimpleLightingGpuProgram();
    shared_ptr<render::GpuProgram> createColoredGpuProgram();
    shared_ptr<render::GpuProgram> createRttQuadProgram();
    shared_ptr<render::GpuProgram> createAmbientPassProgram();
    shared_ptr<render::MeshData> createMeshData(const std::string &meshDataPath, ResourceLoadingMode lm);
    shared_ptr<render::Texture2D> createTexture(const std::string &imagePath, ResourceLoadingMode lm);
    shared_ptr<render::Texture2D> createEmptyTexture(const std::string &id = "");
    shared_ptr<render::TextureCube> createCubeTexture(const std::string &positiveXImage, const std::string &negativeXImage,
                                                      const std::string &positiveYImage, const std::string &negativeYImage,
                                                      const std::string &positiveZImage, const std::string &negativeZImage,
                                                      ResourceLoadingMode lm);
    shared_ptr<render::MaterialLib> createMaterialLib(const std::string &mtlLibPath);

    void loadResources(const std::vector<std::string> &resourcesList, ResourceLoadingMode lm);

    void appendResource(shared_ptr<Resource> resource);

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
    bool isResourceLoaded(const ResourceGUID &guid) const;

    void addListener(Listener *listener);
    void removeListener(Listener *listener);
};

} }
