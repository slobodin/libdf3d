#pragma once

#include "IFileSystem.h"

namespace df3d {

class FileDataSource;

class DF3D_DLL FileSystem : public IFileSystem
{
    std::recursive_mutex m_lock;
    std::unordered_map<std::string, std::string> m_fullPathsCache;

public:
    FileSystem();
    ~FileSystem();

    shared_ptr<FileDataSource> openFile(const std::string &filePath) override;
    bool fileExists(const std::string &filePath) override;

    std::string fullPath(const std::string &path) override;
};

}
