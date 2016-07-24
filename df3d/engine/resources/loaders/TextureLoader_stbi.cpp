#include "TextureLoader_stbi.h"

#ifdef _DEBUG
#define STB_DO_ERROR_PRINT
#endif
#define STB_IMAGE_IMPLEMENTATION
#ifndef STB_DO_ERROR_PRINT
#define STBI_NO_FAILURE_STRINGS     // NOTE: is not thread-safe.
#endif
#include <stb/stb_image.h>

#include "../TextureLoaders.h"
#include <df3d/engine/render/Texture.h>
#include <df3d/engine/io/DataSource.h>

namespace df3d {

// stbi image loader helpers.

// fill 'data' with 'size' bytes.  return number of bytes actually read
static int read(void *user, char *data, int size)
{
    auto dataSource = static_cast<DataSource*>(user);
    return dataSource->read(data, size);
}

// skip the next 'n' bytes, or 'unget' the last -n bytes if negative
static void skip(void *user, int n)
{
    if (n < 0)
    {
        // TODO:
        DF3D_ASSERT_MESS(false, "not implemented");
    }

    auto dataSource = static_cast<DataSource*>(user);
    dataSource->seek(n, SeekDir::CURRENT);
}

// returns nonzero if we are at end of file/data
static int eof(void *user)
{
    auto dataSource = static_cast<DataSource*>(user);
    return dataSource->tell() >= dataSource->getSize();
}

bool TextureLoader_stbi::load(shared_ptr<DataSource> source, bool forceRGBA, PixelData &outPixelData)
{
    outPixelData.data.clear();

    if (!source)
        return false;

    stbi_io_callbacks callbacks;
    callbacks.read = read;
    callbacks.skip = skip;
    callbacks.eof = eof;

    int x, y, bpp;
    auto pixels = stbi_load_from_callbacks(&callbacks, source.get(), &x, &y, &bpp, forceRGBA ? 4 : 0);
    if (!pixels)
    {
        DFLOG_WARN("Can not load image %s", source->getPath().c_str());
#ifdef STB_DO_ERROR_PRINT
        DFLOG_WARN(stbi_failure_reason());
#endif
        return false;
    }

    auto fmt = PixelFormat::INVALID;

    if (bpp == STBI_rgb)
    {
        fmt = PixelFormat::RGB;
    }
    else if (bpp == STBI_rgb_alpha)
    {
        fmt = PixelFormat::RGBA;
    }
    else
    {
        DFLOG_WARN("Parsed image with an invalid bpp");
        stbi_image_free(pixels);
        return false;
    }

    if (forceRGBA)
        DF3D_ASSERT(fmt == PixelFormat::RGBA);

    outPixelData.info.format = fmt;
    outPixelData.info.numMips = 0;
    outPixelData.info.width = x;
    outPixelData.info.height = y;

    int compCount = forceRGBA ? 4 : bpp;

    outPixelData.data.assign(pixels, compCount * x * y);

    stbi_image_free(pixels);

    return true;
}

}
