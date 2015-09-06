#pragma once

#include <resources/Resource.h>

FWD_MODULE_CLASS(resources, DecoderWAV)
FWD_MODULE_CLASS(resources, DecoderOGG)

namespace df3d { namespace audio {

class AudioBuffer : public resources::Resource
{
    friend class resources::DecoderWAV;
    friend class resources::DecoderOGG;

    unsigned m_alBufferId = 0;

    void onDecoded(bool decodeResult) override;

public:
    AudioBuffer();
    ~AudioBuffer();
};

} }
