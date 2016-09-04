#include "DefaultFileDevice.h"

#include "FileDataSource.h"
#include "FileSystemHelpers.h"
#include <df3d/lib/os/PlatformFile.h>

namespace df3d {

DefaultFileDevice::DefaultFileDevice()
{

}

DefaultFileDevice::~DefaultFileDevice()
{

}

shared_ptr<DataSource> DefaultFileDevice::openDataSource(const char *path)
{
    auto platformFile = PlatformOpenFile(path);
    if (!platformFile)
        return nullptr;

    std::string filePath = path;
    FileSystemHelpers::convertSeparators(filePath);

    return make_shared<FileDataSource>(filePath, std::move(platformFile));
}

}
