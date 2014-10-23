#pragma once

#include <CEGUI/ImageCodec.h>

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiImageCodecImpl : public CEGUI::ImageCodec
{
public:
    CeguiImageCodecImpl();

    CEGUI::Texture* load(const CEGUI::RawDataContainer &data, CEGUI::Texture *result);
};

} } }
