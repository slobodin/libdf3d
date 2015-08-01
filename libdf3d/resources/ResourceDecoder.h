#pragma once

namespace df3d { namespace resources {

class FileDataSource;
class Resource;

//! Abstract resource decoder. Decodes and initializes a resource from a given stream.
class ResourceDecoder
{
public:
    ResourceDecoder();
    virtual ~ResourceDecoder();

    //! Creates resource stub which can be immediately used while the resource is actually being decoded.
    virtual shared_ptr<Resource> createResource() = 0;

    //! Performs resource decoding from given stream.
    /*! 
      This function is called by resource manager. Needs to be thread safe
      \param stream Input stream
      \param resource Resource which is need to be initialized. Instantiated by createResource.
      \return Decode result
    */
    virtual bool decodeResource(shared_ptr<FileDataSource> file, shared_ptr<Resource> resource) = 0;

    // TODO:
    // Create function "updateProgress"
};

} }