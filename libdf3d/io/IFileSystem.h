#pragma once

namespace df3d {

class FileDataSource;

class IFileSystem : utils::NonCopyable
{
public:
    IFileSystem() = default;
    virtual ~IFileSystem() = default;

    virtual shared_ptr<FileDataSource> openFile(const std::string &filePath) = 0;
    virtual bool fileExists(const std::string &filePath) = 0;

    virtual std::string fullPath(const std::string &path) = 0;
};

}
