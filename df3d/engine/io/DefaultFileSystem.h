#pragma once

#include "IFileSystem.h"

namespace df3d {

class DF3D_DLL DefaultFileSystem : public IFileSystem
{
    std::recursive_mutex m_lock;
    std::unordered_map<std::string, std::string> m_fullPathsCache;

public:
    DefaultFileSystem();
    ~DefaultFileSystem();

    shared_ptr<DataSource> open(const std::string &filePath) override;
    bool fileExists(const std::string &filePath) override;

    std::string fullPath(const std::string &path) override;
};

}
