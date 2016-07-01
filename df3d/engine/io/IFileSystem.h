#pragma once

namespace df3d {

class DataSource;

class DF3D_DLL IFileSystem : NonCopyable
{
public:
    IFileSystem() = default;
    virtual ~IFileSystem() = default;

    virtual shared_ptr<DataSource> open(const std::string &filePath) = 0;
    virtual bool fileExists(const std::string &filePath) = 0;

    virtual std::string fullPath(const std::string &path) = 0;
};

}
