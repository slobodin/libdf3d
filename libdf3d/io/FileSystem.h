#pragma once

namespace df3d {

class FileDataSource;

class DF3D_DLL FileSystem : utils::NonCopyable
{
    mutable std::recursive_mutex m_lock;
    std::vector<std::string> m_searchPaths;

    mutable std::unordered_map<std::string, std::string> m_fullPathsCache;

    static std::string canonicalPath(const std::string &rawPath);

public:
    FileSystem();
    ~FileSystem();

    shared_ptr<FileDataSource> openFile(const std::string &filePath);

    void addSearchPath(const std::string &path);

    std::string fullPath(const std::string &path) const;

    static std::string getFileDirectory(const std::string &filePath);
    static std::string pathConcatenate(const std::string &fp1, const std::string &fp2);
    static std::string getFileExtension(const std::string &rawPath);
    static std::string getFilename(const std::string &filePath);
    static std::string getFilenameWithoutExtension(const std::string &filePath);
};

}