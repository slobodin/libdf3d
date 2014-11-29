#include "df3d_pch.h"
#include "DecoderImage.h"

#include <resources/FileDataSource.h>
#include <resources/FileSystem.h>
#include <render/Image.h>

#define STB_IMAGE_IMPLEMENTATION
#ifndef STB_DO_ERROR_PRINT
#define STBI_NO_FAILURE_STRINGS     // not thread-safe
#endif
#include <stb/stb_image.h>

namespace df3d { namespace resources {

// stb image loader helpers.
namespace
{
    // fill 'data' with 'size' bytes.  return number of bytes actually read 
    int read(void *user, char *data, int size)
    {
        auto dataSource = static_cast<FileDataSource*>(user);
        return dataSource->getRaw(data, size);
    }

    // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
    void skip(void *user, int n)
    {
        if (n < 0)
        {
            // TODO:
            assert(false);
        }

        auto dataSource = static_cast<FileDataSource*>(user);
        dataSource->seek(n, std::ios_base::cur);
    }

    // returns nonzero if we are at end of file/data
    int eof(void *user)
    {
        auto dataSource = static_cast<FileDataSource*>(user);
        return dataSource->tell() >= dataSource->getSize();
    }
}

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

    stbi_io_callbacks callbacks;
    callbacks.read = read;
    callbacks.skip = skip;
    callbacks.eof = eof;
    auto dataSource = file.get();

    int x, y, bpp;
    auto pixels = stbi_load_from_callbacks(&callbacks, dataSource, &x, &y, &bpp, 0);
    if (!pixels)
    {
        base::glog << "Can not load image" << file->getPath() << base::logwarn;
#ifdef STB_DO_ERROR_PRINT
        base::glog << stbi_failure_reason() << base::logwarn;
#endif
        return false;
    }

    auto fmt = render::Image::Format::INVALID;

    if (bpp == STBI_rgb)
        fmt = render::Image::Format::RGB;
    else if (bpp == STBI_rgb_alpha)
        fmt = render::Image::Format::RGBA;
    else if (bpp == STBI_grey)
        fmt = render::Image::Format::GRAYSCALE;
    else
        base::glog << "Parsed image with an invalid bpp" << base::logwarn;

    texture->setWithData(pixels, x, y, fmt);

    stbi_image_free(pixels);

    return true;
}

} }