#include "FileSystem.h"

#include "FileDataSource.h"
#include "FileSystemHelpers.h"

namespace df3d {

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{

}

shared_ptr<FileDataSource> FileSystem::openFile(const std::string &filePath)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto fullP = fullPath(filePath);
    if (fullP.empty())
        return nullptr;

    return FileSystemHelpers::openFile(fullP);
}

bool FileSystem::fileExists(const std::string &filePath)
{
    return openFile(filePath) != nullptr;
}

std::string FileSystem::fullPath(const std::string &path)
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
