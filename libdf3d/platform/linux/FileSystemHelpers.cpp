#include "df3d_pch.h"
#include "../FileSystemHelpers.h"

#include <platform/desktop_common/FileDataSourceDesktop.h>
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

shared_ptr<resources::FileDataSource> FileSystemHelpers::openFile(const std::string &path)
{
    auto result = make_shared<platform_impl::FileDataSourceDesktop>(path.c_str());
    if (!result->valid())
        return nullptr;
    return result;
}

} }
