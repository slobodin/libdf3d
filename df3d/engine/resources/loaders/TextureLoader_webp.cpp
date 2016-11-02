#include "TextureLoader_webp.h"

#include <webp/decode.h>
#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/TextureResource.h>

namespace df3d {

TextureResourceData* TextureLoader_webp(ResourceDataSource &dataSource, Allocator &alloc, bool forceRGBA)
{
    PodArray<uint8_t> webpData(alloc);
    webpData.resize(dataSource.getSize());
    dataSource.read(&webpData[0], webpData.size());

    WebPBitstreamFeatures features;
    if (WebPGetFeatures(webpData.data(), webpData.size(), &features) != VP8_STATUS_OK)
    {
        DFLOG_WARN("Failed to load a texture: WebPGetInfo failed");
        return nullptr;
    }

    bool result = false;
    auto resource = alloc.makeNew<TextureResourceData>(alloc);
    if (features.has_alpha || forceRGBA)
    {
        resource->info.format = PixelFormat::RGBA;
        resource->info.numMips = 0;
        resource->info.width = features.width;
        resource->info.height = features.height;

        resource->pixels.resize(features.width * features.height * 4);

        result = WebPDecodeRGBAInto(webpData.data(), webpData.size(), &resource->pixels[0], resource->pixels.size(), features.width * 4) != nullptr;
    }
    else
    {
        resource->info.format = PixelFormat::RGB;
        resource->info.numMips = 0;
        resource->info.width = features.width;
        resource->info.height = features.height;

        resource->pixels.resize(features.width * features.height * 3);

        result = WebPDecodeRGBInto(webpData.data(), webpData.size(), &resource->pixels[0], resource->pixels.size(), features.width * 3) != nullptr;
    }

    if (!result)
    {
        DFLOG_WARN("Failed to decode WEBP data");
        alloc.makeDelete(resource);
        return nullptr;
    }

    return resource;
}

}
