#include "df3d_pch.h"
#include "CeguiImageCodecImpl.h"

#include <CEGUI/Exceptions.h>

namespace df3d { namespace gui { namespace cegui_impl {

using namespace CEGUI;

CeguiImageCodecImpl::CeguiImageCodecImpl()
    : ImageCodec("CeguiImageCodecImpl - libdf3d ImageCodec implementation.")
{
    d_supportedFormat = "png jpg bmp";
}

CEGUI::Texture* CeguiImageCodecImpl::load(const CEGUI::RawDataContainer &data, CEGUI::Texture *result)
{
    CEGUI_THROW(InvalidRequestException("ImageCodec for libdf3d is not implemented."));

    return nullptr;
}

} } }