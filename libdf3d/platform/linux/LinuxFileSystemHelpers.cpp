#include "df3d_pch.h"
#include "LinuxFileSystemHelpers.h"

#include <unistd.h>

namespace df3d { namespace platform {

bool FileSystemHelpers::isPathAbsolute(const std::string &path)
{
    if (path.empty())
        return false;

    return path[0] == '/';
}

bool FileSystemHelpers::pathExists(const std::string &path)
{
    return access(path.c_str(), F_OK) != -1;
}

} }
