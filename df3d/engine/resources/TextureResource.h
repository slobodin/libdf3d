#pragma once

#include <df3d/engine/render/RenderCommon.h>
#include "IResourceHolder.h"

namespace df3d {

struct TextureInfo
{
    PixelFormat format = PixelFormat::INVALID;
    size_t width = 0;
    size_t height = 0;
    size_t numMips = 0;
};

struct TextureResourceData : private NonCopyable
{
    PodArray<uint8_t> pixels;
    TextureInfo info;

    TextureResourceData(Allocator &alloc) : pixels(alloc) { }
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
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

// Workaround for turbobadger.
TextureResourceData* LoadTexture_Workaround(ResourceDataSource &dataSource, Allocator &alloc);

}