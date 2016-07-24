#include "TextureLoader_webp.h"

#include "../TextureLoaders.h"
#include <webp/decode.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/Texture.h>
#include <df3d/engine/io/DataSource.h>

namespace df3d {

bool TextureLoader_webp::load(shared_ptr<DataSource> source, bool forceRGBA, PixelData &outPixelData)
{
    outPixelData.data.clear();

    if (!source)
        return false;

    PodArray<uint8_t> webpData(MemoryManager::allocDefault());
    webpData.resize(source->getSize());
    source->read(&webpData[0], webpData.size());

    WebPBitstreamFeatures features;
    if (WebPGetFeatures(webpData.data(), webpData.size(), &features) != VP8_STATUS_OK)
    {
        DFLOG_WARN("Failed to load a texture: WebPGetInfo failed");
        return false;
    }

    bool result = false;
    if (features.has_alpha || forceRGBA)
    {
        outPixelData.info.format = PixelFormat::RGBA;
        outPixelData.info.numMips = 0;
        outPixelData.info.width = features.width;
        outPixelData.info.height = features.height;

        outPixelData.data.resize(features.width * features.height * 4);

        result = WebPDecodeRGBAInto(webpData.data(), webpData.size(), &outPixelData.data[0], outPixelData.data.size(), features.width * 4) != nullptr;
    }
    else
    {
        outPixelData.info.format = PixelFormat::RGB;
        outPixelData.info.numMips = 0;
        outPixelData.info.width = features.width;
        outPixelData.info.height = features.height;

        outPixelData.data.resize(features.width * features.height * 3);

        result = WebPDecodeRGBInto(webpData.data(), webpData.size(), &outPixelData.data[0], outPixelData.data.size(), features.width * 3) != nullptr;
    }

    if (!result)
    {
        DFLOG_WARN("Failed to decode WEBP data");
        outPixelData.data.clear();
        return false;
    }

    return true;
}

}
