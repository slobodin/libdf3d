#pragma once

namespace df3d { namespace resources {

class DF3D_DLL Resource : utils::NonCopyable
{
protected:
    ResourceGUID m_guid;

    std::atomic<bool> m_initialized = false;
    std::atomic<bool> m_resident = false;

public:
    Resource();
    virtual ~Resource() = default;

    void setGUID(const ResourceGUID &guid);
    const ResourceGUID &getGUID() const;

    const std::string &getFilePath() const;

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
    ResourceLoadingMode loadingMode = ResourceLoadingMode::IMMEDIATE;

    virtual ~FSResourceLoader() = default;

    virtual Resource* createDummy() = 0;
    virtual void decode(shared_ptr<FileDataSource> source, Resource *resource) = 0;
};

bool IsGUIDValid(const ResourceGUID &guid);
ResourceGUID CreateGUIDFromPath(const std::string &path);

} }
