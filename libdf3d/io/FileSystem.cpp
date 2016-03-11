#include "FileSystem.h"

#include "FileDataSource.h"
#include "FileSystemHelpers.h"
#include "MemoryDataSource.h"
#include <libdf3d/resources/ResourceContainer.h>
#include <libdf3d/utils/Utils.h>

namespace df3d {

std::string FileSystem::canonicalPath(const std::string &rawPath)
{
    if (rawPath.empty())
        return "";

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

shared_ptr<FileDataSource> FileSystem::openFile(const std::string &filePath)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto fullP = fullPath(filePath);
    if (fullP.empty())
        return nullptr;

    // Try to find in resource containers.
    auto found = m_resContainerLookup.find(fullP);
    if (found != m_resContainerLookup.end())
    {
        auto archiveFile = found->second.container->getFile();
        const auto &entry = found->second.container->getEntries().at(found->second.entryIdx);

        // We do not return this container because we want thread safety.
        // FIXME: figure out how to avoid copying memory and get concurrent access to a resource pack file.
        auto resContainer = make_shared<ResourceContainerDataSource>(archiveFile, entry);

        auto size = resContainer->getSizeInBytes();

        //glog << "Allocating" << size << "bytes for" << entry.fileName << "from pack" << archiveFile->getPath() << logdebug;

        auto buffer = make_unique<unsigned char[]>(size);
        resContainer->getRaw(buffer.get(), size);

        return make_shared<MemoryDataSource>(std::move(buffer), size, entry.fileName);
    }

    // Try to find in regular file system.
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

    auto fullP = canonicalPath(path);
    if (!fullP.empty())
    {
        m_fullPathsCache[path] = fullP;
        return fullP;
    }

    return "";
}

bool FileSystem::addDirectoryResourceContainer(const std::string &dirPath)
{
    DF3D_ASSERT(false, "not implemented");

    return false;
}

bool FileSystem::addArchiveResourceContainer(const std::string &archivePath)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    auto archiveFullP = fullPath(archivePath);
    if (archiveFullP.empty())
        return false;

    auto archiveFile = FileSystemHelpers::openFile(archiveFullP);
    if (!archiveFile)
        return false;

    auto containerFound = std::find_if(m_resourceContainers.begin(), m_resourceContainers.end(), 
                                       [&archiveFullP](const unique_ptr<ResourceContainer> &cont) {
        return cont->getFile()->getPath() == archiveFullP;
    });

    if (containerFound != m_resourceContainers.end())
    {
        glog << "Resource container" << archiveFullP << "already added" << logwarn;
        return false;
    }

    auto resContainer = make_unique<ResourceContainer>(archiveFile);

    for (size_t i = 0; i < resContainer->getEntries().size(); i++)
    {
        ResourceContainerCachedEntry cachedEntry;
        cachedEntry.container = resContainer.get();
        cachedEntry.entryIdx = i;

        auto resPath = std::string(resContainer->getEntries()[i].fileName);

        resPath = fullPath(resPath);
        if (resPath.empty())
        {
            glog << "Invalid entry path in a resource container" << logwarn;
            continue;
        }

#ifdef _DEBUG
        if (utils::contains_key(m_resContainerLookup, resPath))
            glog << "A resource with path" << resPath << "already presented in cache" << logwarn;
#endif

        m_resContainerLookup[resPath] = cachedEntry;
    }

    m_resourceContainers.push_back(std::move(resContainer));

    return true;
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
