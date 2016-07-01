#include "DefaultFileSystem.h"

#include "FileDataSource.h"
#include "FileSystemHelpers.h"
#include <df3d/lib/os/PlatformFile.h>

namespace df3d {

DefaultFileSystem::DefaultFileSystem()
{

}

DefaultFileSystem::~DefaultFileSystem()
{

}

shared_ptr<DataSource> DefaultFileSystem::open(const std::string &filePath)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto fullP = fullPath(filePath);
    if (fullP.empty())
        return nullptr;

    auto platformFile = PlatformOpenFile(filePath.c_str());
    if (!platformFile)
        return nullptr;

    return make_shared<FileDataSource>(fullP, std::move(platformFile));
}

bool DefaultFileSystem::fileExists(const std::string &filePath)
{
    return PlatformFileExists(filePath.c_str());
}

std::string DefaultFileSystem::fullPath(const std::string &path)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto foundInCache = m_fullPathsCache.find(path);
    if (foundInCache != m_fullPathsCache.end())
        return foundInCache->second;

    auto fullP = FileSystemHelpers::canonicalPath(path);
    if (!fullP.empty())
    {
        m_fullPathsCache[path] = fullP;
        return fullP;
    }

    return "";
}

}
