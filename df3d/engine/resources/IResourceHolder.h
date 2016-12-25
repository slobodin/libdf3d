#pragma once

namespace df3d {

class ResourceDataSource;
class Allocator;

class IResourceHolder
{
public:
    IResourceHolder() = default;
    virtual ~IResourceHolder() = default;

    virtual void listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps) = 0;
    virtual bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) = 0;
    virtual void decodeCleanup(Allocator &allocator) = 0;
    virtual bool createResource(Allocator &allocator) = 0;
    virtual void destroyResource(Allocator &allocator) = 0;

    virtual void* getResource() = 0;
};

}
