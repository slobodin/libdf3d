#pragma once

namespace df3d {

class DataSource;
class ResourceManager;

class DF3D_DLL Resource : NonCopyable
{
    friend class ResourceManager;

protected:
    ResourceGUID m_guid;

    std::atomic<bool> m_initialized;
    std::atomic<bool> m_resident;

public:
    Resource();
    virtual ~Resource() = default;

    void setGUID(const ResourceGUID &guid);
    const ResourceGUID& getGUID() const;

    const std::string& getFilePath() const;

    bool isInitialized() const { return m_initialized; }
    bool isResident() const { return m_resident; }

    void setResident(bool resident = true) { m_resident = resident; }
};

class ManualResourceLoader
{
public:
    virtual ~ManualResourceLoader() = default;

    //! Should return fully initialized resource, created by `new'.
    //! NOTE: no shared_ptr because of covariant return types.
    virtual Resource* load() = 0;
};

class FSResourceLoader
{
public:
    const ResourceLoadingMode loadingMode;

    FSResourceLoader(ResourceLoadingMode lm) : loadingMode(lm) { }
    virtual ~FSResourceLoader() = default;

    //! Creates resource stub which can be immediately used while the resource is actually being decoded.
    virtual Resource* createDummy() = 0;
    //! Performs resource decoding from given source. Must be thread safe.
    virtual bool decode(shared_ptr<DataSource> source) = 0;
    //! Resource manager calls this when decoding is complete. Called from main thread only. 
    virtual void onDecoded(Resource *resource) = 0;
};

bool IsGUIDValid(const ResourceGUID &guid);
ResourceGUID CreateGUIDFromPath(const std::string &path);

}
