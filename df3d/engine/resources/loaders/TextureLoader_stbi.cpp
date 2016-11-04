#include "TextureLoader_stbi.h"

#ifdef _DEBUG
#define STB_DO_ERROR_PRINT
#endif
#define STB_IMAGE_IMPLEMENTATION
#ifndef STB_DO_ERROR_PRINT
#define STBI_NO_FAILURE_STRINGS     // NOTE: is not thread-safe.
#endif
#include <stb/stb_image.h>

#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/TextureResource.h>

namespace df3d {

// stbi image loader helpers.

// fill 'data' with 'size' bytes.  return number of bytes actually read
static int read(void *user, char *data, int size)
{
    auto dataSource = (ResourceDataSource*)user;
    return dataSource->read(data, size);
}

// skip the next 'n' bytes, or 'unget' the last -n bytes if negative
static void skip(void *user, int n)
{
    if (n < 0)
    {
        // TODO:
        DF3D_FATAL("not implemented");
    }

    auto dataSource = (ResourceDataSource*)user;
    dataSource->seek(n, SeekDir::CURRENT);
}

// returns nonzero if we are at end of file/data
static int eof(void *user)
{
    auto dataSource = (ResourceDataSource*)user;
    return dataSource->tell() >= dataSource->getSize();
}

TextureResourceData* TextureLoader_stbi(ResourceDataSource &dataSource, Allocator &alloc, bool forceRGBA)
{
    stbi_io_callbacks callbacks;
    callbacks.read = read;
    callbacks.skip = skip;
    callbacks.eof = eof;

    int x, y, bpp;
    auto pixels = stbi_load_from_callbacks(&callbacks, &dataSource, &x, &y, &bpp, forceRGBA ? 4 : 0);
    if (!pixels)
    {
#ifdef STB_DO_ERROR_PRINT
        DFLOG_WARN(stbi_failure_reason());
#endif
        return nullptr;
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
        return nullptr;
    }

    if (forceRGBA)
        DF3D_ASSERT(fmt == PixelFormat::RGBA);

    auto resource = MAKE_NEW(alloc, TextureResourceData)(alloc);
    resource->info.format = fmt;
    resource->info.numMips = 0;
    resource->info.width = x;
    resource->info.height = y;

    int compCount = forceRGBA ? 4 : bpp;

    resource->pixels.assign(pixels, compCount * x * y);

    stbi_image_free(pixels);

    return resource;
}

}
