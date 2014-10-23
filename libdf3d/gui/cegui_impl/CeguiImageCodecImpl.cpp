#include "df3d_pch.h"
#include "CeguiImageCodecImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

CeguiImageCodecImpl::CeguiImageCodecImpl()
    : ImageCodec("CeguiImageCodecImpl - libdf3d ImageCodec implementation.")
{
    d_supportedFormat = "png jpg bmp";
}

CEGUI::Texture* CeguiImageCodecImpl::load(const CEGUI::RawDataContainer &data, CEGUI::Texture *result)
{
    return nullptr;
}

} } }