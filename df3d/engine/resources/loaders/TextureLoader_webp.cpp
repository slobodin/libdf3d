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
    auto resource = MAKE_NEW(alloc, TextureResourceData);
    resource->mipLevels.resize(1);
    resource->mipLevels[0].width = features.width;
    resource->mipLevels[0].height = features.height;

    auto &pixels = resource->mipLevels[0].pixels;

    if (features.has_alpha || forceRGBA)
    {
        resource->format = PixelFormat::RGBA;

        pixels.resize(features.width * features.height * 4);

        result = WebPDecodeRGBAInto(webpData.data(), webpData.size(), &pixels[0], pixels.size(), features.width * 4) != nullptr;
    }
    else
    {
        resource->format = PixelFormat::RGB;

        pixels.resize(features.width * features.height * 3);

        result = WebPDecodeRGBInto(webpData.data(), webpData.size(), &pixels[0], pixels.size(), features.width * 3) != nullptr;
    }

    if (!result)
    {
        DFLOG_WARN("Failed to decode WEBP data");
        MAKE_DELETE(alloc, resource);
        return nullptr;
    }

    return resource;
}

}
