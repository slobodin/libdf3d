#pragma once

namespace df3d { namespace resources {

class DF3D_DLL Resource : utils::NonCopyable
{
protected:
    ResourceGUID m_guid;

    std::atomic<bool> m_initialized;
    std::atomic<bool> m_resident;

public:
    Resource();
    virtual ~Resource() { }

    void setGUID(const ResourceGUID &guid);
    const ResourceGUID &getGUID() const;

    const std::string &getFilePath() const;

    bool isValid() const { return m_initialized; }
    bool isResident() const { return m_resident; }

    void setInitialized(bool initialized = true) { m_initialized = initialized; }
    void setResident(bool resident = true) { m_resident = resident; }
};

bool IsGUIDValid(const ResourceGUID &guid);
ResourceGUID CreateGUIDFromPath(const std::string &path);

} }
