#include "df3d_pch.h"
#include "../FileSystemHelpers.h"

#include <resources/FileDataSource.h>
#include <jni.h>

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

    bool retRes = false;

    if (isPathAbsolute(path))
    {
        auto file = fopen(path.c_str(), "r");
        if (file)
        {
            retRes = true;
            fclose(file);
        }
    }
    else
    {
        // AAssetManager_open()
    }

    return retRes;
}

} }
