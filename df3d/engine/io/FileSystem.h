#pragma once

namespace df3d {

class DataSource;

class DF3D_DLL IFileDevice : NonCopyable
{
public:
    IFileDevice() = default;
    virtual ~IFileDevice() = default;

    virtual shared_ptr<DataSource> openDataSource(const char *path) = 0;
};

class DF3D_DLL FileSystem : NonCopyable
{
    std::recursive_mutex m_lock;

    unique_ptr<IFileDevice> m_fileDevice;

public:
    FileSystem();
    ~FileSystem();

    void setFileDevice(unique_ptr<IFileDevice> &&fd) { m_fileDevice = std::move(fd); }

    shared_ptr<DataSource> open(const char *path);
};

}
