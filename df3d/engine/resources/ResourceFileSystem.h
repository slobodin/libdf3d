#pragma once

#include <df3d/Common.h>
#include <df3d/lib/NonCopyable.h>

namespace df3d {

class ResourceDataSource;

class ResourceFileSystem : NonCopyable
{
public:
    ResourceFileSystem() = default;
    virtual ~ResourceFileSystem() = default;

    virtual ResourceDataSource* open(const char *path) = 0;
    virtual void close(ResourceDataSource *dataSource) = 0;
};

unique_ptr<ResourceFileSystem> CreateDefaultResourceFileSystem();

}
