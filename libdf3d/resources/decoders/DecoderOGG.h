#pragma once

#include "../ResourceDecoder.h"

namespace df3d { namespace resources {

class DecoderOGG : public ResourceDecoder
{
public:
    DecoderOGG();
    ~DecoderOGG();

    shared_ptr<Resource> createResource() override;
    bool decodeResource(const shared_ptr<FileDataSource> file, shared_ptr<Resource> resource) override;
};

} }