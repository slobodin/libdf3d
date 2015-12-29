#include <io/FileSystemHelpers.h>

#include <platform/desktop_common/FileDataSourceDesktop.h>
#include <unistd.h>

namespace df3d {

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

shared_ptr<FileDataSource> FileSystemHelpers::openFile(const std::string &path)
{
    return make_shared<platform_impl::FileDataSourceDesktop>(path);
}

}
