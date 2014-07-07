#include "df3d_pch.h"
#include "DecoderImage.h"

#include <SDL_image.h>
#include <SDL_rwops.h>
#include <resources/FileDataSource.h>
#include <resources/FileSystem.h>
#include <render/Image.h>

namespace df3d { namespace resources {

DecoderImage::DecoderImage()
{
}

DecoderImage::~DecoderImage()
{
}

shared_ptr<Resource> DecoderImage::createResource()
{
    return make_shared<render::Image>();
}

bool DecoderImage::decodeResource(const shared_ptr<FileDataSource> file, shared_ptr<Resource> resource)
{
    if (!file || !file->valid())
        return false;

    auto texture = boost::dynamic_pointer_cast<render::Image>(resource);
    if (!texture)
        return false;

    auto ext = FileSystem::getFileExtension(file->getPath());
    ext.erase(0, 1);        // remove the dot
    boost::to_upper(ext);

    SDL_Surface *surf = IMG_LoadTyped_RW(file->getSdlRwops(), 0, ext.c_str());
    if (!surf)
    {
        base::glog << "Can not load SDL surface" << file->getPath() << base::logwarn;
        base::glog << SDL_GetError() << base::logwarn;

        return false;
    }

    // FIXME:
    // Format
    render::Image::PixelFormat pf = render::Image::PF_INVALID;
    if (surf->format->BytesPerPixel == 1)
        pf = render::Image::PF_GRAYSCALE;
    if (surf->format->BytesPerPixel == 3)
        pf = render::Image::PF_RGB;
    else if (surf->format->BytesPerPixel == 4)
        pf = render::Image::PF_RGBA;

    SDL_LockSurface(surf);
    texture->setWithData((const unsigned char *)surf->pixels, surf->w, surf->h, surf->pitch, pf);
    SDL_UnlockSurface(surf);

    SDL_FreeSurface(surf);

    return true;
}

} }