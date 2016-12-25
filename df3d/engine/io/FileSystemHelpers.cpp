#include "FileSystemHelpers.h"

#include <df3d/lib/Utils.h>

namespace df3d {

std::string FileSystemHelpers::getFileDirectory(const std::string &filePath)
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

std::string FileSystemHelpers::pathConcatenate(const std::string &fp1, const std::string &fp2)
{
    if (fp1.empty())
        return fp2;
    if (fp2.empty())
        return fp1;

    /*
    if (FileSystemHelpers::isPathAbsolute(fp2))
        return "";
    */

    if (fp2[0] == '\\' || fp2[0] == '/')
        return "";

    auto result = fp1;
    if (result.back() != '/' && result.back() != '\\')
        result.push_back('/');

    return result += fp2;
}

std::string FileSystemHelpers::getFilename(const std::string &filePath)
{
    auto lastSlashPos = std::find_if(filePath.rbegin(), filePath.rend(), [](char ch) { return ch == '/' || ch == '\\'; });
    if (lastSlashPos == filePath.rend())
        return filePath;

    return std::string(lastSlashPos.base(), filePath.end());
}

std::string FileSystemHelpers::getFilenameWithoutExtension(const std::string &filePath)
{
    auto filename = getFilename(filePath);
    if (filename.empty())
        return "";

    auto dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos)
        return "";

    return std::string(filename.cbegin(), filename.cbegin() + dotPos);
}

std::string FileSystemHelpers::canonicalPath(const std::string &rawPath)
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

void FileSystemHelpers::convertSeparators(std::string &str)
{
    std::replace(str.begin(), str.end(), '\\', '/');
}

bool FileSystemHelpers::compareExtension(const char *path, const char *extension)
{
    if (!(path && extension))
        return false;

    auto len1 = strlen(path);
    auto len2 = strlen(extension);

    for (size_t i = 0; i < std::min(len1, len2); i++)
    {
        auto s1 = path + len1 - i;
        auto s2 = extension + len2 - i;
        if (*s1 != *s2)
            return false;
    }

    return true;
}

}
