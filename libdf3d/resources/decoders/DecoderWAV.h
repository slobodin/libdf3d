#pragma once

#include "../ResourceDecoder.h"

namespace df3d { namespace resources {

class DecoderWAV : public ResourceDecoder
{
public:
    DecoderWAV();
    ~DecoderWAV();

    shared_ptr<Resource> createResource() override;
    bool decodeResource(shared_ptr<FileDataSource> file, shared_ptr<Resource> resource) override;
};

} }
