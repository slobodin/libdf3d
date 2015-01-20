#include "df3d_pch.h"
#include "WindowsPhoneFileSystemHelpers.h"

namespace df3d { namespace platform {

bool FileSystemHelpers::isPathAbsolute(const std::string &path)
{
    if (path.size() < 2)
        return false;

    return path[1] == ':' && ((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' || path[0] <= 'Z'));
}

bool FileSystemHelpers::pathExists(const std::string &path)
{
    return false;
}

} }
