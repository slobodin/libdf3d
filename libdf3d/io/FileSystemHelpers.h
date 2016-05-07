#pragma once

namespace df3d {

class FileDataSource;

struct DF3D_DLL FileSystemHelpers
{
    static bool isPathAbsolute(const std::string &path);
    static bool pathExists(const std::string &path);
    static shared_ptr<FileDataSource> openFile(const std::string &path);

    static std::string getFileDirectory(const std::string &filePath);
    static std::string pathConcatenate(const std::string &fp1, const std::string &fp2);
    static std::string getFileExtension(const std::string &rawPath);
    static std::string getFilename(const std::string &filePath);
    static std::string getFilenameWithoutExtension(const std::string &filePath);
    static std::string canonicalPath(const std::string &rawPath);
    static void convertSeparators(std::string &str);
};

}
