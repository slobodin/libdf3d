#include "FileSystem.h"

#include "FileDataSource.h"
#include <libdf3d/io/FileSystemHelpers.h>
#include <libdf3d/utils/Utils.h>

namespace df3d {

std::string FileSystem::canonicalPath(const std::string &rawPath)
{
    if (rawPath.empty())
        return "";

    // NOTE:
    // Can't AAssetManager_open from paths like 'foo1/../bar1/foo/'
#ifndef DF3D_ANDROID
    if (!FileSystemHelpers::pathExists(rawPath))
        return "";
#endif

    auto result = rawPath;

    // \\ -> /
    std::replace(result.begin(), result.end(), '\\', '/');

    // ///// -> /
    auto bothSlashes = [](char a, char b) -> bool { return a == '/' && b == '/'; };
    result.erase(std::unique(result.begin(), result.end(), bothSlashes), result.end());

    // Remove not needed ./
    utils::replace_all(result, "/./", "/");
    if (utils::starts_with(result, "./"))
        result.erase(0, 2);
    if (utils::ends_with(result, "/."))
        result.erase(result.size() - 2, 2);

    // Remove /../
    int lastSlashPos = 0;
    int i = 0;
    while (i < (int)result.size() - 2)
    {
        if (result[i] == '/' && result[i + 1] != '.' && result[i + 2] != '.')
            lastSlashPos = i;
        else if (result[i] == '/' && result[i + 1] == '.' && result[i + 2] == '.')
        {
            result.erase(lastSlashPos, i - lastSlashPos + 3);
            i = lastSlashPos;
            continue;
        }
        i++;
    }

    // FIXME:
    if (result.size() >= 2 && result[0] == '/' && result[1] != '/')
        result.erase(0, 1);

    return result;
}

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{

}

shared_ptr<FileDataSource> FileSystem::openFile(const std::string &filePath) const
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto fullP = fullPath(filePath);
    if (fullP.empty())
        return nullptr;

    auto fileSource = FileSystemHelpers::openFile(fullP);
    if (!fileSource->valid())
        return nullptr;

    return fileSource;
}

bool FileSystem::fileExists(const std::string &filePath) const
{
    return openFile(filePath) != nullptr;
}

std::string FileSystem::fullPath(const std::string &path) const
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto foundInCache = m_fullPathsCache.find(path);
    if (foundInCache != m_fullPathsCache.end())
        return foundInCache->second;

    auto fullP = canonicalPath(path);
    if (!fullP.empty())
    {
        m_fullPathsCache[path] = fullP;
        return fullP;
    }

    return "";
}

std::string FileSystem::getFileDirectory(const std::string &filePath)
{
    auto fullPath = canonicalPath(filePath);
    if (fullPath.empty())
        return "";

    std::string::iterator lastSlashPos = fullPath.end();
    for (auto it = fullPath.rbegin(); it != fullPath.rend(); it++)
    {
        if (*it == '/' || *it == '\\')
        {
            lastSlashPos = it.base();
            break;
        }
    }

    if (lastSlashPos == fullPath.end())
        return "";

    return std::string(fullPath.begin(), lastSlashPos);
}

std::string FileSystem::pathConcatenate(const std::string &fp1, const std::string &fp2)
{
    if (fp1.empty())
        return fp2;
    if (fp2.empty())
        return fp1;

    if (FileSystemHelpers::isPathAbsolute(fp2))
        return "";

    if (fp2[0] == '\\' || fp2[0] == '/')
        return "";

    auto result = fp1;
    if (result.back() != '/' && result.back() != '\\')
        result.push_back('/');

    return result += fp2;
}

std::string FileSystem::getFileExtension(const std::string &rawPath)
{
    auto dotPos = rawPath.find_last_of('.');
    if (dotPos == std::string::npos)
        return "";

    auto ext = std::string(rawPath.begin() + dotPos, rawPath.end());
    utils::to_lower(ext);

    return ext;
}

std::string FileSystem::getFilename(const std::string &filePath)
{
    auto lastSlashPos = std::find_if(filePath.rbegin(), filePath.rend(), [](char ch) { return ch == '/' || ch == '\\'; });
    if (lastSlashPos == filePath.rend())
        return filePath;

    return std::string(lastSlashPos.base(), filePath.end());
}

std::string FileSystem::getFilenameWithoutExtension(const std::string &filePath)
{
    auto filename = getFilename(filePath);
    if (filename.empty())
        return "";

    auto dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos)
        return "";

    return std::string(filename.cbegin(), filename.cbegin() + dotPos);
}

}
