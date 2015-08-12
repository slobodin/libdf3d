#pragma once

#include "../ResourceDecoder.h"

namespace df3d { namespace resources {

//! DFMesh file format decoder.
/*!
 * DFMesh is a file format for meshes used in libdf3d.
 * Basically it contains a number of submesh vertex/index data with corresponding material.
 * It may include convex hull for physics support.
 * TODO: more features.
 */
class DecoderDFMesh : public ResourceDecoder
{
public:
    DecoderDFMesh();
    ~DecoderDFMesh();

    shared_ptr<Resource> createResource() override;
    bool decodeResource(shared_ptr<FileDataSource> file, shared_ptr<Resource> resource) override;
};

} }
