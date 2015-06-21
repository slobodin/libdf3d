#pragma once

#include <base/TypeDefs.h>

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

class DF3D_DLL ResourceManager : boost::noncopyable
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

    Listener *m_listener = nullptr;

    ResourceManager();
    ~ResourceManager();

    shared_ptr<Resource> findResource(const std::string &guid) const;
    shared_ptr<ResourceDecoder> getDecoder(const std::string &extension) const;

    shared_ptr<Resource> loadResourceFromFileSystem(const char *path, ResourceLoadingMode lm);

public:
    shared_ptr<audio::AudioBuffer> createAudioBuffer(const char *audioPath);
    shared_ptr<render::GpuProgram> createGpuProgram(const char *vertexShader, const char *fragmentShader);
    shared_ptr<render::GpuProgram> createSimpleLightingGpuProgram();
    shared_ptr<render::GpuProgram> createColoredGpuProgram();
    shared_ptr<render::GpuProgram> createRttQuadProgram();
    shared_ptr<render::GpuProgram> createAmbientPassProgram();
    shared_ptr<render::MeshData> createMeshData(const char *meshDataPath, ResourceLoadingMode lm);
    shared_ptr<render::Texture2D> createTexture(const char *imagePath, ResourceLoadingMode lm);
    shared_ptr<render::Texture2D> createEmptyTexture(const char *id = nullptr);
    shared_ptr<render::TextureCube> createCubeTexture(const char *positiveXImage, const char *negativeXImage, 
                                                      const char *positiveYImage, const char *negativeYImage, 
                                                      const char *positiveZImage, const char *negativeZImage, 
                                                      ResourceLoadingMode lm);
    shared_ptr<render::MaterialLib> createMaterialLib(const char *mtlLibPath);

    void loadResources(const std::vector<std::string> &resourcesList, ResourceLoadingMode lm);

    void appendResource(shared_ptr<Resource> resource);

    void unloadResource(const ResourceGUID &guid);
    void unloadResource(shared_ptr<Resource> resource);
    void unloadUnused();

    bool resourceExists(const ResourceGUID &guid) const;
    bool resourceLoaded(const ResourceGUID &guid) const;

    void setListener(Listener *listener) { m_listener = listener; }
};

} }
