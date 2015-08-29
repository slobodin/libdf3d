#pragma once

namespace df3d { namespace resources {

class DF3D_DLL Resource : utils::NonCopyable
{
protected:
    ResourceGUID m_guid;

    std::atomic<bool> m_initialized = false;
    std::atomic<bool> m_resident = false;

    //! Called by resource decoder. This function should set m_initialized depending on decode result.
    virtual void onDecoded(bool decodeResult) = 0;

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

bool IsGUIDValid(const ResourceGUID &guid);
ResourceGUID CreateGUIDFromPath(const std::string &path);

} }
