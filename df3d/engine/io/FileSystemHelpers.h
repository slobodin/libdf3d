#pragma once

namespace df3d {

class FileSystemHelpers
{
public:
    static std::string getFileDirectory(const std::string &filePath);
    static std::string pathConcatenate(const std::string &fp1, const std::string &fp2);
    static std::string getFilename(const std::string &filePath);
    static std::string getFilenameWithoutExtension(const std::string &filePath);
    static std::string canonicalPath(const std::string &rawPath);
    static void convertSeparators(std::string &str);

    static bool compareExtension(const char *path, const char *extension);
};

}
