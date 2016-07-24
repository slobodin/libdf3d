#pragma once

#include <df3d/engine/resources/Resource.h>
#include <df3d/engine/render/Texture.h>

namespace df3d {

struct PixelData : private NonCopyable
{
    PodArray<uint8_t> data;
    TextureInfo info;

    PixelData();
    ~PixelData() = default;
};

class Texture2DManualLoader : public ManualResourceLoader
{
    TextureInfo m_info;
    const void *m_data;
    size_t m_dataSize;

public:
    Texture2DManualLoader(const TextureInfo &info, const void *data, size_t dataSize);

    Texture* load() override;
};

class Texture2DFSLoader : public FSResourceLoader
{
    std::string m_pathToTexture;
    PixelData m_data;

public:
    Texture2DFSLoader(const std::string &path, uint32_t flags, ResourceLoadingMode lm);

    Texture* createDummy() override;
    bool decode(shared_ptr<DataSource> source) override;
    void onDecoded(Resource *resource) override;
};

// Workaround for turbobadger!
bool GetPixelBufferFromSource(shared_ptr<DataSource> source, bool forceRGBA, PixelData &outPixelData);

}
