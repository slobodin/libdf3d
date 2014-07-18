#pragma once

#include <resources/Resource.h>

FWD_MODULE_CLASS(components, TextMeshComponent)
FWD_MODULE_CLASS(resources, DecoderTTF)

namespace df3d { namespace gui {

class FontFace : public resources::Resource
{
    friend class components::TextMeshComponent;
    friend class resources::DecoderTTF;

public:
    FontFace();
    ~FontFace();

    bool init();
};

} }