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

    auto texture = dynamic_pointer_cast<render::Image>(resource);
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

    texture->setWithData(surf);

    SDL_FreeSurface(surf);

    return true;
}

} }