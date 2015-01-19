#include "df3d_pch.h"
#include "WindowsFileSystemHelpers.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace df3d { namespace platform {

bool FileSystemHelpers::isPathAbsolute(const std::string &path)
{
    if (path.size() < 2)
        return false;

    return path[1] == ':' && ((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' || path[0] <= 'Z'));
}

bool FileSystemHelpers::pathExists(const std::string &path)
{
    DWORD attrs = GetFileAttributes(path.c_str());

    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

} }

// TODO:
// Android:
// I place it here for a while.
//bool isPathAbsolute(const std::string &path)
//{
//    if (path.empty())
//        return false;
//
//    return path[0] == '/';
//}

//bool FileSystem::pathExists(const std::string &path)
//{
//#if defined(DF3D_WINDOWS)
//
//#elif defined(__ANDROID__)
//    if (path.empty())
//        return false;
//
//    // TODO:
//    // Search in assets folder first.
//    return FileDataSource(path.c_str()).valid();