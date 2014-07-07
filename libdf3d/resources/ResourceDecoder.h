#pragma once

namespace df3d { namespace resources {

class FileDataSource;
class Resource;

class ResourceDecoder
{
public:
    ResourceDecoder();
    virtual ~ResourceDecoder();

    virtual shared_ptr<Resource> createResource() = 0;

    // Need to be thread-safe.
    virtual bool decodeResource(const shared_ptr<FileDataSource> file, shared_ptr<Resource> resource) = 0;

    // TODO:
    // Create function "updateProgress"
};

} }