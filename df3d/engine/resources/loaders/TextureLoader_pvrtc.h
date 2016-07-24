#pragma once

namespace df3d {

class DataSource;
struct PixelData;

class TextureLoader_pvrtc
{
public:
    static bool load(shared_ptr<DataSource> source, PixelData &outPixelData);
};

}
