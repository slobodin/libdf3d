#pragma once

namespace df3d {

class DataSource;
struct PixelData;

class TextureLoader_webp
{
public:
    static bool load(shared_ptr<DataSource> source, bool forceRGBA, PixelData &outPixelData);
};

}
