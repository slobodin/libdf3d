#include "df3d_pch.h"
#include "FileSystem.h"

#if defined(DF3D_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined(DF3D_WINDOWS_PHONE)
#include <wrl.h>
#endif

#include "FileDataSource.h"

namespace df3d { namespace resources {

bool isPathAbsolute(const std::string &path)
{
#if defined(DF3D_WINDOWS) || defined(DF3D_WINDOWS_PHONE)
    if (path.size() < 2)
        return false;

    return path[1] == ':' && ((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' || path[0] <= 'Z'));
#elif defined(__ANDROID__)
    if (path.empty())
        return false;

    return path[0] == '/';
#else
#error Not implemented.
#endif
}

FileSystem::FileSystem()
{
    m_searchPaths.push_back("");
}

FileSystem::~FileSystem()
{
}

std::string FileSystem::canonicalPath(const std::string &rawPath)
{
    if (rawPath.empty())
        return "";

#if defined(DF3D_WINDOWS) || defined(DF3D_WINDOWS_PHONE)
    if (!pathExists(rawPath))
        return "";
#endif

    auto result = rawPath;

    // \\ -> /
    std::replace(result.begin(), result.end(), '\\', '/');

    // ///// -> /
    auto bothSlashes = [](char a, char b) -> bool { return a == '/' && b == '/'; };
    result.erase(std::unique(result.begin(), result.end(), bothSlashes), result.end());

    // Remove not needed ./
    boost::replace_all(result, "/./", "/");
    if (boost::starts_with(result, "./"))
        result.erase(0, 2);
    if (boost::ends_with(result, "/."))
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

#if defined(__ANDROID__)
    if (!pathExists(result))
        return "";
#endif

    return result;
}

bool FileSystem::pathExists(const std::string &path)
{
#if defined(DF3D_WINDOWS)
    DWORD attrs = GetFileAttributes(path.c_str());

    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
#elif defined(__ANDROID__)
    if (path.empty())
        return false;

    // TODO:
    // Search in assets folder first.
    return FileDataSource(path.c_str()).valid();
#elif defined(DF3D_WINDOWS_PHONE)
    return false;
#else
#error Not implemented.
#endif
}

shared_ptr<FileDataSource> FileSystem::openFile(const char *filePath)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto fullP = fullPath(filePath);
    if (fullP.empty())
        return nullptr;

    auto fileSource = make_shared<FileDataSource>(fullP.c_str());
    if (!fileSource->valid())
        return nullptr;

    return fileSource;
}

void FileSystem::addSearchPath(const char *path)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto found = std::find_if(m_searchPaths.cbegin(), m_searchPaths.cend(), [&](const std::string &it) { return it == path; });
    if (found != m_searchPaths.cend())
    {
        base::glog << "Trying to add duplicate search path" << path << base::logwarn;
        return;
    }

    m_searchPaths.push_back(path);
}

std::string FileSystem::fullPath(const char *path) const
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto foundInCache = m_fullPathsCache.find(path);
    if (foundInCache != m_fullPathsCache.end())
        return foundInCache->second;

    for (const auto &sp : m_searchPaths)
    {
        auto newpath = pathConcatenate(sp, path);
        auto fullp = canonicalPath(newpath);
        if (!fullp.empty())
        {
            m_fullPathsCache[path] = fullp;
            return fullp;
        }
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

    if (isPathAbsolute(fp2))
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
    boost::algorithm::to_lower(ext);

    return ext;
}

std::string FileSystem::getFilename(const std::string &filePath)
{
    auto lastSlashPos = std::find_if(filePath.rbegin(), filePath.rend(), [](char ch) { return ch == '/' || ch == '\\'; });
    if (lastSlashPos == filePath.rend())
        return "";

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

} }
