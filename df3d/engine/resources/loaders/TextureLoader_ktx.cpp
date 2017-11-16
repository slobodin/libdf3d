#include "TextureLoader_ktx.h"

#include <df3d/engine/resources/ResourceDataSource.h>
#include <df3d/engine/resources/TextureResource.h>

#if defined(DF3D_IOS)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <df3d/engine/render/gl/RenderBackendGL.h>
#endif

namespace df3d {

namespace {

uint8_t KTX_MAGIC[4] = { 0xAB, 'K', 'T', 'X' };

#pragma pack(push, 1)
struct KTXHeader
{
    uint8_t identifier[12];

    uint32_t endianness;

    uint32_t glType;
    uint32_t glTypeSize;
    uint32_t glFormat;
    uint32_t glInternalFormat;
    uint32_t glBaseInternalFormat;
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth;
    uint32_t numberOfArrayElements;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmapLevels;
    uint32_t bytesOfKeyValueData;
};
#pragma pack(pop)

bool CheckSupported(const KTXHeader &header)
{
    if (0x04030201 != header.endianness)
    {
        DFLOG_WARN("Unsupported endianness");
        return false;
    }

    if (header.numberOfMipmapLevels == 0)
    {
        DFLOG_WARN("KTX texture should be mipmapped!");
        return false;
    }

    if (header.numberOfArrayElements > 0)
    {
        DFLOG_WARN("Unsupported KTX file");
        return false;
    }

    if (!(header.glType == 0 && header.glFormat == 0))
    {
        DFLOG_WARN("KTX file must be compressed texture");
        return false;
    }

    if (header.pixelDepth != 0)
    {
        DFLOG_WARN("KTX implementation supports only 2D textures");
        return false;
    }

    if (header.numberOfFaces > 1)
    {
        DFLOG_WARN("KTX implementation doesn't support cube maps");
        return false;
    }

    return true;
}

}

TextureResourceData* TextureLoader_ktx(ResourceDataSource &dataSource, Allocator &alloc)
{
    KTXHeader header;
    dataSource.read(&header, sizeof(KTXHeader));

    if (memcmp(header.identifier, KTX_MAGIC, sizeof(KTX_MAGIC)) != 0)
    {
        DFLOG_WARN("Invalid KTX magic");
        return nullptr;
    }

    if (!CheckSupported(header))
        return nullptr;

    dataSource.seek(header.bytesOfKeyValueData, SeekDir::CURRENT);

    auto resource = MAKE_NEW(alloc, TextureResourceData);
    resource->format = PixelFormat::KTX;

    if (header.glBaseInternalFormat == GL_RGB)
    {
        resource->glBaseInternalFormat = header.glBaseInternalFormat;
    }
    else
    {
        DF3D_ASSERT(false);
        return nullptr;
    }
    resource->glInternalFormat = header.glInternalFormat;

    void *buffer = nullptr;
    uint32_t bufferSize = 0;

    resource->mipLevels.resize(header.numberOfMipmapLevels);

    for (uint32_t level = 0; level < header.numberOfMipmapLevels; ++level)
    {
        uint32_t pixelWidth = std::max(1u, header.pixelWidth >> level);
        uint32_t pixelHeight = std::max(1u, header.pixelHeight >> level);

        uint32_t faceLodSize = 0;
        dataSource.read(&faceLodSize, sizeof(uint32_t));

        uint32_t faceLodSizeRounded = (faceLodSize + 3) & ~(uint32_t)3;

        if (!buffer)
        {
            buffer = malloc(faceLodSizeRounded);
            bufferSize = faceLodSizeRounded;
        }
        else if (bufferSize < faceLodSizeRounded)
        {
            DFLOG_WARN("Subsequent levels cannot be larger than the first level");
            break;
        }

        dataSource.read(buffer, faceLodSizeRounded);

        auto &mipLevel = resource->mipLevels[level];

        mipLevel.width = pixelWidth;
        mipLevel.height = pixelHeight;
        mipLevel.pixels.resize(faceLodSize);
        memcpy(mipLevel.pixels.data(), buffer, faceLodSize);
    }

    free(buffer);

    return resource;
}

}
