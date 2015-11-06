#include "../FileSystemHelpers.h"

#include <platform/desktop_common/FileDataSourceDesktop.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace df3d {

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

shared_ptr<FileDataSource> FileSystemHelpers::openFile(const std::string &path)
{
    return make_shared<platform::FileDataSourceDesktop>(path.c_str());
}

}
