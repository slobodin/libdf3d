#include "df3d_pch.h"
#include "../FileSystemHelpers.h"

#include <resources/FileDataSource.h>

namespace df3d { namespace platform {

bool FileSystemHelpers::isPathAbsolute(const std::string &path)
{
    if (path.empty())
        return false;

    return path[0] == '/';
}

bool FileSystemHelpers::pathExists(const std::string &path)
{
    if (path.empty())
        return false;
    // TODO:
    // Search in assets folder first.
    return resources::FileDataSource(path.c_str()).valid();
}

} }
