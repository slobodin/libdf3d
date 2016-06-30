#include <libdf3d/df3d.h>
#include <libdf3d/io/FileSystemHelpers.h>

#import <Foundation/Foundation.h>
#include <libdf3d/platform/desktop_common/FileDataSourceDesktop.h>

namespace df3d {

static NSString* GetBundlePath(const std::string &path)
{
    NSString *pathNs = [NSString stringWithUTF8String:path.c_str()];
    return [[NSBundle mainBundle] pathForResource:pathNs ofType:nil];
}

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

    NSString *bundlePath = GetBundlePath(path);

    return fopen([bundlePath UTF8String], "rb") != nullptr;
}

shared_ptr<FileDataSource> FileSystemHelpers::openFile(const std::string &path)
{
    NSString *bundlePath = GetBundlePath(path);

    auto result = make_shared<platform_impl::FileDataSourceDesktop>([bundlePath UTF8String]);
    if (!result->valid())
        return nullptr;

    return result;
}

}
