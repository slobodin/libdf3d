#pragma once

namespace df3d {

class FileDataSource;
class ResourceContainer;

class DF3D_DLL FileSystem : utils::NonCopyable
{
    std::recursive_mutex m_lock;
    std::unordered_map<std::string, std::string> m_fullPathsCache;

    static std::string canonicalPath(const std::string &rawPath);

    struct ResourceContainerCachedEntry
    {
        ResourceContainer *container = nullptr;
        size_t entryIdx = 0;
    };

    std::unordered_map<std::string, ResourceContainerCachedEntry> m_resContainerLookup;
    std::vector<unique_ptr<ResourceContainer>> m_resourceContainers;

public:
    FileSystem();
    ~FileSystem();

    shared_ptr<FileDataSource> openFile(const std::string &filePath);
    bool fileExists(const std::string &filePath);

    std::string fullPath(const std::string &path);

    bool addDirectoryResourceContainer(const std::string &dirPath);
    bool addArchiveResourceContainer(const std::string &archivePath);

    static std::string getFileDirectory(const std::string &filePath);
    static std::string pathConcatenate(const std::string &fp1, const std::string &fp2);
    static std::string getFileExtension(const std::string &rawPath);
    static std::string getFilename(const std::string &filePath);
    static std::string getFilenameWithoutExtension(const std::string &filePath);
};

}
