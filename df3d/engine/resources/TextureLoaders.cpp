#include "TextureLoaders.h"

#include <zlib.h>
#include <webp/decode.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/io/DefaultFileSystem.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/io/DataSource.h>
#include <df3d/lib/JsonUtils.h>

#ifdef _DEBUG
#define STB_DO_ERROR_PRINT
#endif

#define STB_IMAGE_IMPLEMENTATION
#ifndef STB_DO_ERROR_PRINT
#define STBI_NO_FAILURE_STRINGS     // not thread-safe
#endif
#include <stb/stb_image.h>

namespace df3d {

// stb image loader helpers.

// fill 'data' with 'size' bytes.  return number of bytes actually read
static int read(void *user, char *data, int size)
{
    auto dataSource = static_cast<DataSource*>(user);
    return dataSource->read(data, size);
}

// skip the next 'n' bytes, or 'unget' the last -n bytes if negative
static void skip(void *user, int n)
{
    if (n < 0)
    {
        // TODO:
        DF3D_ASSERT_MESS(false, "not implemented");
    }

    auto dataSource = static_cast<DataSource*>(user);
    dataSource->seek(n, SeekDir::CURRENT);
}

// returns nonzero if we are at end of file/data
static int eof(void *user)
{
    auto dataSource = static_cast<DataSource*>(user);
    return dataSource->tell() >= dataSource->getSize();
}

static unique_ptr<PixelBuffer> LoadWebp(shared_ptr<DataSource> source, bool forceRgba)
{
    PodArray<uint8_t> pixels(MemoryManager::allocDefault());
    pixels.resize(source->getSize());
    source->read(&pixels[0], pixels.size());

    WebPBitstreamFeatures features;
    if (WebPGetFeatures(pixels.data(), pixels.size(), &features) != VP8_STATUS_OK)
    {
        DFLOG_WARN("Failed to load a texture: WebPGetInfo failed");
        return nullptr;
    }

    PixelFormat format;
    size_t decodedSize = 0;
    uint8_t *decoded = nullptr;
    bool result = false;

    if (features.has_alpha || forceRgba)
    {
        decodedSize = features.width * features.height * 4;
        format = PixelFormat::RGBA;
        decoded = new uint8_t[decodedSize];

        result = WebPDecodeRGBAInto(pixels.data(), pixels.size(), decoded, decodedSize, features.width * 4) != nullptr;
    }
    else
    {
        decodedSize = features.width * features.height * 3;
        format = PixelFormat::RGB;
        decoded = new uint8_t[decodedSize];

        result = WebPDecodeRGBInto(pixels.data(), pixels.size(), decoded, decodedSize, features.width * 3) != nullptr;
    }

    if (!result)
    {
        DFLOG_WARN("Failed to decode WEBP data");
        delete[] decoded;
        return nullptr;
    }

    return make_unique<PixelBuffer>(features.width, features.height, decoded, format, false);
}

static size_t ComputePVRTCDataSize(int width, int height, int bpp)
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

static unique_ptr<PixelBuffer> LoadPvr(shared_ptr<DataSource> source)
{
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

    PvrtcHeader header;
    source->read(&header, sizeof(header));

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

    int bpp;
    switch (header.pixelFormat[0])
    {
    case 0:     // PVRTC 2bpp RGB
        bpp = 2;
        break;
    case 1:     // PVRTC 2bpp RGBA
        bpp = 2;
        break;
    case 2:     // PVRTC 4bpp RGB
        bpp = 4;
        break;
    case 3:     // PVRTC 4bpp RGBA
        bpp = 4;
        break;
    default:
        DFLOG_WARN("Failed to decode PVRTC texture. Unknown texture format");
        return nullptr;
    }

    int width = header.width;
    int height = header.height;
    int numMips = header.numMips;

    // Skip meta-data.
    source->seek(header.metaDataSize, SeekDir::CURRENT);

    // Determine pixels size.
    int w = width;
    int h = height;
    size_t dataSize = 0;
    for (uint32_t mip = 0; mip < header.numMips; ++mip)
    {
        dataSize += ComputePVRTCDataSize(w, h, bpp) * header.numFaces;
        w = std::max(w >> 1, 1);
        h = std::max(h >> 1, 1);
    }

    PodArray<uint8_t> pixels(MemoryManager::allocDefault());
    pixels.resize(dataSize);
    if (source->read(&pixels[0], dataSize) != dataSize)
    {
        DFLOG_WARN("Failed to read PVRTC pixels");
        return nullptr;
    }

    return nullptr;
}

static unique_ptr<PixelBuffer> LoadPixelBuffer(shared_ptr<DataSource> source, bool forceRgba = false)
{
    if (!source)
    {
        DFLOG_WARN("Failed to load pixel buffer from file source. Source is null.");
        return nullptr;
    }

    const auto ext = FileSystemHelpers::getFileExtension(source->getPath());
    if (ext == ".webp")
        return LoadWebp(source, forceRgba);
    else if (ext == ".pvr")
        return LoadPvr(source);

    stbi_io_callbacks callbacks;
    callbacks.read = read;
    callbacks.skip = skip;
    callbacks.eof = eof;
    auto dataSource = source.get();

    int x, y, bpp;
    auto pixels = stbi_load_from_callbacks(&callbacks, dataSource, &x, &y, &bpp, 0);
    if (!pixels)
    {
        DFLOG_WARN("Can not load image %s", source->getPath().c_str());
#ifdef STB_DO_ERROR_PRINT
        DFLOG_WARN(stbi_failure_reason());
#endif
        return nullptr;
    }

    auto fmt = PixelFormat::INVALID;

    if (bpp == STBI_rgb)
        fmt = PixelFormat::RGB;
    else if (bpp == STBI_rgb_alpha)
        fmt = PixelFormat::RGBA;
    else
        DFLOG_WARN("Parsed image with an invalid bpp");

    auto result = make_unique<PixelBuffer>(x, y, pixels, fmt);

    stbi_image_free(pixels);

    return result;
}

Texture2DManualLoader::Texture2DManualLoader(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params)
    : m_pixelBuffer(std::move(pixelBuffer)), m_params(params)
{

}

Texture* Texture2DManualLoader::load()
{
    auto handle = svc().renderManager().getBackend().createTexture2D(
        m_pixelBuffer->getWidth(),
        m_pixelBuffer->getHeight(),
        m_pixelBuffer->getFormat(),
        m_pixelBuffer->getData(),
        m_params);

    if (!handle.valid())
        return nullptr;

    TextureInfo info;
    info.width = m_pixelBuffer->getWidth();
    info.height = m_pixelBuffer->getHeight();
    info.sizeInBytes = m_pixelBuffer->getSizeInBytes();
    info.isCubemap = false;

    return new Texture(handle, info);
}

Texture2DFSLoader::Texture2DFSLoader(const std::string &path, const TextureCreationParams &params, ResourceLoadingMode lm)
    : FSResourceLoader(lm),
    m_pathToTexture(path),
    m_params(params)
{

}

Texture* Texture2DFSLoader::createDummy()
{
    return new Texture();
}

bool Texture2DFSLoader::decode(shared_ptr<DataSource> source)
{
    m_pixelBuffer = LoadPixelBuffer(source);
    return m_pixelBuffer != nullptr;
}

void Texture2DFSLoader::onDecoded(Resource *resource)
{
    auto texture = static_cast<Texture*>(resource);

    auto handle = svc().renderManager().getBackend().createTexture2D(
        m_pixelBuffer->getWidth(), 
        m_pixelBuffer->getHeight(),
        m_pixelBuffer->getFormat(),
        m_pixelBuffer->getData(),
        m_params);
    if (handle.valid())
    {
        texture->setHandle(handle);

        TextureInfo info;
        info.width = m_pixelBuffer->getWidth();
        info.height = m_pixelBuffer->getHeight();
        info.sizeInBytes = m_pixelBuffer->getSizeInBytes();
        info.isCubemap = false;

        texture->setTextureInfo(info);
    }

    // Explicitly remove CPU copy of pixel buffer in order to prevent caching.
    // Instead, will load new copy from FS when rebinding occurs.
    m_pixelBuffer.reset();
}

TextureCubeFSLoader::TextureCubeFSLoader(const std::string &path, const TextureCreationParams &params, ResourceLoadingMode lm)
    : FSResourceLoader(lm),
    m_jsonPath(path),
    m_params(params)
{

}

Texture* TextureCubeFSLoader::createDummy()
{
    return new Texture();
}

bool TextureCubeFSLoader::decode(shared_ptr<DataSource> source)
{
    std::string buffer(source->getSize(), 0);
    source->read(&buffer[0], buffer.size());

    auto jsonRoot = JsonUtils::fromSource(buffer);
    if (jsonRoot.empty())
        return false;

    auto srcPathDir = FileSystemHelpers::getFileDirectory(source->getPath());

    auto getSource = [&srcPathDir](const std::string &texturePath)
    {
        auto fullPath = FileSystemHelpers::pathConcatenate(srcPathDir, texturePath);
        return svc().fileSystem().open(fullPath);
    };

    m_pixelBuffers[(size_t)CubeFace::POSITIVE_X] = LoadPixelBuffer(getSource(jsonRoot["positive_x"].asString()));
    m_pixelBuffers[(size_t)CubeFace::NEGATIVE_X] = LoadPixelBuffer(getSource(jsonRoot["negative_x"].asString()));
    m_pixelBuffers[(size_t)CubeFace::POSITIVE_Y] = LoadPixelBuffer(getSource(jsonRoot["positive_y"].asString()));
    m_pixelBuffers[(size_t)CubeFace::NEGATIVE_Y] = LoadPixelBuffer(getSource(jsonRoot["negative_y"].asString()));
    m_pixelBuffers[(size_t)CubeFace::POSITIVE_Z] = LoadPixelBuffer(getSource(jsonRoot["positive_z"].asString()));
    m_pixelBuffers[(size_t)CubeFace::NEGATIVE_Z] = LoadPixelBuffer(getSource(jsonRoot["negative_z"].asString()));

    for (auto &pb : m_pixelBuffers)
    {
        if (!pb)
            return false;
    }

    return true;
}

void TextureCubeFSLoader::onDecoded(Resource *resource)
{
    for (const auto &pb : m_pixelBuffers)
    {
        if (!pb)
        {
            DFLOG_WARN("Failed to decode cube texture. Image(s) invalid.");
            return;
        }
    }

    auto texture = static_cast<Texture*>(resource);
    auto handle = svc().renderManager().getBackend().createTextureCube(m_pixelBuffers, m_params);
    if (handle.valid())
    {
        texture->setHandle(handle);

        TextureInfo info;
        // FIXME:
        info.width = 0;
        info.height = 0;
        info.isCubemap = true;

        texture->setTextureInfo(info);
    }

    // Explicitly clean up main memory.
    for (int i = 0; i < (size_t)CubeFace::COUNT; i++)
        m_pixelBuffers[i].reset();
}

unique_ptr<PixelBuffer> GetPixelBufferFromSource(shared_ptr<DataSource> source, bool forceRgba)
{
    return LoadPixelBuffer(source, forceRgba);
}

}
