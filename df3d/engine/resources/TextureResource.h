#pragma once

#include <df3d/engine/render/RenderCommon.h>
#include "IResourceHolder.h"

namespace df3d {

struct TextureResourceData : private NonCopyable
{
    struct MipLevel
    {
        std::vector<uint8_t> pixels;
        size_t width = 0;
        size_t height = 0;
    };

    std::vector<MipLevel> mipLevels;
    PixelFormat format = PixelFormat::INVALID;

    // KTX data.
    uint32_t glInternalFormat = 0;
    uint32_t glBaseInternalFormat = 0;
};

struct TextureResource
{
    TextureHandle handle;
    size_t width = 0;
    size_t height = 0;
};

class TextureHolder : public IResourceHolder
{
    TextureResourceData *m_resourceData = nullptr;
    TextureResource *m_resource = nullptr;
    uint32_t m_flags = 0;

public:
    void listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps) override { }
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

// Workaround for turbobadger.
TextureResourceData* LoadTexture_Workaround(ResourceDataSource &dataSource, Allocator &alloc);

}
