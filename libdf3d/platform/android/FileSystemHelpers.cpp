#include "df3d_pch.h"
#include "../FileSystemHelpers.h"

#include <platform/android/FileDataSourceAndroid.h>

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
        auto aassetManager = FileDataSourceAndroid::getAAssetManager();
        auto file = AAssetManager_open(aassetManager, path.c_str(), AASSET_MODE_UNKNOWN);
        if (file)
        {
            retRes = true;
            AAsset_close(file);
        }
    }

    return retRes;
}

shared_ptr<resources::FileDataSource> FileSystemHelpers::openFile(const std::string &path)
{
    if (isPathAbsolute(path))
    {
        // TODO:
        // Return desktop version.
        assert(false);
        return nullptr;
    }
    else
    {
        return make_shared<platform::FileDataSourceAndroid>(path.c_str());
    }
}

} }
