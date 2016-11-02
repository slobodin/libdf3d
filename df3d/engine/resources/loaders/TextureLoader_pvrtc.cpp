#include "TextureLoader_pvrtc.h"

#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/TextureResource.h>

namespace df3d {

struct PvrtcHeader
{
    uint32_t version;
    uint32_t flags;
    uint32_t pixelFormat[2];
    uint32_t colorSpace;
    uint32_t channelType;
    uint32_t height;
    uint32_t width;
    uint32_t depth;
    uint32_t numSurfaces;
    uint32_t numFaces;
    uint32_t numMips;
    uint32_t metaDataSize;
};

size_t ComputePVRTCDataSize(int width, int height, int bpp)
{
    int blockSize;
    int widthBlocks;
    int heightBlocks;

    if (bpp == 4)
    {
        blockSize = 4 * 4;
        widthBlocks = std::max(width >> 2, 2);
        heightBlocks = std::max(height >> 2, 2);
    }
    else
    {
        blockSize = 8 * 4;
        widthBlocks = std::max(width >> 3, 2);
        heightBlocks = std::max(height >> 2, 2);
    }

    return widthBlocks * heightBlocks * ((blockSize  * bpp) >> 3);
}

TextureResourceData* TextureLoader_pvrtc(ResourceDataSource &dataSource, Allocator &alloc)
{
    PvrtcHeader header;
    dataSource.read(&header, sizeof(header));

    if (header.version != 0x03525650)
    {
        DFLOG_WARN("Unsupported PVRTC version");
        return nullptr;
    }

    if (header.pixelFormat[1] != 0)
    {
        DFLOG_WARN("Failed to decode PVRTC texture. Pixel format is not supported");
        return nullptr;
    }

    if (header.numFaces != 1)
    {
        DFLOG_WARN("Cubemaps not supported for PVRTC");
        return nullptr;
    }

    PixelFormat pixelFormat = PixelFormat::INVALID;

    int bpp;
    switch (header.pixelFormat[0])
    {
    case 0:     // PVRTC 2bpp RGB
        bpp = 2;
        pixelFormat = PixelFormat::PVRTC_2RGB_V1;
        break;
    case 1:     // PVRTC 2bpp RGBA
        bpp = 2;
        pixelFormat = PixelFormat::PVRTC_2RGBA_V1;
        break;
    case 2:     // PVRTC 4bpp RGB
        bpp = 4;
        pixelFormat = PixelFormat::PVRTC_4RGB_V1;
        break;
    case 3:     // PVRTC 4bpp RGBA
        bpp = 4;
        pixelFormat = PixelFormat::PVRTC_4RGBA_V1;
        break;
    default:
        DFLOG_WARN("Failed to decode PVRTC texture. Unknown texture format");
        return nullptr;
    }

    // Skip meta-data.
    dataSource.seek(header.metaDataSize, SeekDir::CURRENT);

    // Determine pixels size.
    int w = header.width;
    int h = header.height;
    size_t dataSize = 0;
    for (uint32_t mip = 0; mip < header.numMips; ++mip)
    {
        dataSize += ComputePVRTCDataSize(w, h, bpp) * header.numFaces;
        w = std::max(w >> 1, 1);
        h = std::max(h >> 1, 1);
    }

    auto resource = alloc.makeNew<TextureResourceData>(alloc);
    resource->pixels.resize(dataSize);
    if (dataSource.read(&resource->pixels[0], dataSize) != dataSize)
    {
        DFLOG_WARN("Failed to read PVRTC pixels");
        alloc.makeDelete(resource);
        return nullptr;
    }

    resource->info.format = pixelFormat;
    resource->info.width = header.width;
    resource->info.height = header.height;
    resource->info.numMips = header.numMips;

    return resource;
}

}
