#include "df3d_pch.h"
#include "CeguiImageCodecImpl.h"

#include <CEGUI/Exceptions.h>
#include <base/Controller.h>
#include <resources/ResourceManager.h>
#include <render/Image.h>
// FIXME:
// Decoders supposed to be used only by resource manager.
#include <resources/decoders/DecoderImage.h>

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
    //auto textureImage = g_resourceManager->getResource<render::Image>(path.c_str());
    //if (!textureImage)
    //    return nullptr;

    //auto texture = make_shared<render::Texture>();
    //texture->setImage(textureImage);


    return nullptr;
}

} } }