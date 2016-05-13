#include "TextureLoaders.h"

#include <zlib.h>
#include <webp/decode.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/render/RenderManager.h>
#include <libdf3d/render/IRenderBackend.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/io/FileSystemHelpers.h>
#include <libdf3d/io/FileDataSource.h>
#include <libdf3d/utils/JsonUtils.h>

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
    auto dataSource = static_cast<FileDataSource*>(user);
    return dataSource->getRaw(data, size);
}

// skip the next 'n' bytes, or 'unget' the last -n bytes if negative
static void skip(void *user, int n)
{
    if (n < 0)
    {
        // TODO:
        DF3D_ASSERT(false, "not implemented");
    }

    auto dataSource = static_cast<FileDataSource*>(user);
    dataSource->seek(n, std::ios_base::cur);
}

// returns nonzero if we are at end of file/data
static int eof(void *user)
{
    auto dataSource = static_cast<FileDataSource*>(user);
    return dataSource->tell() >= dataSource->getSizeInBytes();
}

static unique_ptr<PixelBuffer> LoadRaw(shared_ptr<FileDataSource> source)
{
#pragma pack(push, 1)
    struct RawHeader
    {
        char magic[4];
        uint32_t format;
        uint32_t width;
        uint32_t height;
    };
#pragma pack(pop)

    RawHeader header;
    memset(&header, 0, sizeof(header));

    source->getRaw(&header, sizeof(header));

    if (strncmp(header.magic, "raw.", 4) != 0)
    {
        glog << "Invalid raw header" << logwarn;
        return nullptr;
    }

    PixelFormat fmt = PixelFormat::INVALID;
    if (header.format == 0)
        fmt = PixelFormat::RGBA;
    else if (header.format == 1)
        fmt = PixelFormat::RGB;
    else
        glog << "Invalid raw pixel format" << logwarn;

    if (fmt == PixelFormat::INVALID)
        return nullptr;

    const size_t compressedSize = source->getSizeInBytes() - sizeof(header);
    unique_ptr<uint8_t[]> compressedData(new uint8_t[compressedSize]);

    const size_t uncompressedSize = header.height * header.width * GetPixelSizeForFormat(fmt);
    uint8_t *uncompressedData = new uint8_t[uncompressedSize];

    source->seek(sizeof(header), std::ios_base::beg);
    source->getRaw(compressedData.get(), compressedSize);

    uLongf uncompressedSizeGot = uncompressedSize;
    if (uncompress(uncompressedData, &uncompressedSizeGot, compressedData.get(), compressedSize) != Z_OK)
    {
        glog << "Failed to uncompress pixels" << logwarn;
        delete[] uncompressedData;
        return nullptr;
    }

    DF3D_ASSERT(uncompressedSizeGot == uncompressedSize, "sanity check");

    return make_unique<PixelBuffer>(header.width, header.height, uncompressedData, fmt, false);
}

static unique_ptr<PixelBuffer> LoadWebp(shared_ptr<FileDataSource> source, bool forceRgba)
{
    std::vector<uint8_t> pixels(source->getSizeInBytes());
    source->getRaw(&pixels[0], pixels.size());

    WebPBitstreamFeatures features;
    if (WebPGetFeatures(pixels.data(), pixels.size(), &features) != VP8_STATUS_OK)
    {
        glog << "Failed to load a texture: WebPGetInfo failed" << logwarn;
        return nullptr;
    }

    PixelFormat format;
    int width = -1, height = -1;
    uint8_t *decoded = nullptr;
    if (features.has_alpha || forceRgba)
    {
        decoded = WebPDecodeRGBA(pixels.data(), pixels.size(), &width, &height);
        format = PixelFormat::RGBA;
    }
    else
    {
        decoded = WebPDecodeRGB(pixels.data(), pixels.size(), &width, &height);
        format = PixelFormat::RGB;
    }

    if (!decoded)
    {
        glog << "Failed to decode a webp texture" << logwarn;
        return nullptr;
    }

    auto result = make_unique<PixelBuffer>(width, height, decoded, format);

    WebPFree(decoded);

    return result;
}

static unique_ptr<PixelBuffer> LoadPixelBuffer(shared_ptr<FileDataSource> source, bool forceRgba = false)
{
    if (!source)
    {
        glog << "Failed to load pixel buffer from file source. Source is null." << logwarn;
        return nullptr;
    }

    const auto ext = FileSystemHelpers::getFileExtension(source->getPath());
    if (ext == ".webp")
        return LoadWebp(source, forceRgba);
    else if (ext == ".raw")
        return LoadRaw(source);

    stbi_io_callbacks callbacks;
    callbacks.read = read;
    callbacks.skip = skip;
    callbacks.eof = eof;
    auto dataSource = source.get();

    int x, y, bpp;
    auto pixels = stbi_load_from_callbacks(&callbacks, dataSource, &x, &y, &bpp, 0);
    if (!pixels)
    {
        glog << "Can not load image" << source->getPath() << logwarn;
#ifdef STB_DO_ERROR_PRINT
        glog << stbi_failure_reason() << logwarn;
#endif
        return nullptr;
    }

    auto fmt = PixelFormat::INVALID;

    if (bpp == STBI_rgb)
        fmt = PixelFormat::RGB;
    else if (bpp == STBI_rgb_alpha)
        fmt = PixelFormat::RGBA;
    else
        glog << "Parsed image with an invalid bpp" << logwarn;

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
    auto descr = svc().renderManager().getBackend().createTexture2D(
        m_pixelBuffer->getWidth(),
        m_pixelBuffer->getHeight(),
        m_pixelBuffer->getFormat(),
        m_pixelBuffer->getData(),
        m_params);

    if (!descr.valid())
        return nullptr;

    TextureInfo info;
    info.width = m_pixelBuffer->getWidth();
    info.height = m_pixelBuffer->getHeight();
    info.sizeInBytes = m_pixelBuffer->getSizeInBytes();
    info.isCubemap = false;

    return new Texture(descr, info);
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

bool Texture2DFSLoader::decode(shared_ptr<FileDataSource> source)
{
    m_pixelBuffer = LoadPixelBuffer(source);
    return m_pixelBuffer != nullptr;
}

void Texture2DFSLoader::onDecoded(Resource *resource)
{
    auto texture = static_cast<Texture*>(resource);

    auto descr = svc().renderManager().getBackend().createTexture2D(
        m_pixelBuffer->getWidth(), 
        m_pixelBuffer->getHeight(),
        m_pixelBuffer->getFormat(),
        m_pixelBuffer->getData(),
        m_params);
    if (descr.valid())
    {
        texture->setDescriptor(descr);

        TextureInfo info;
        info.width = m_pixelBuffer->getWidth();
        info.height = m_pixelBuffer->getHeight();
        info.sizeInBytes = m_pixelBuffer->getSizeInBytes();
        info.isCubemap = false;

        texture->setTextureInfo(info);
    }

    /*
    glog << "Cleaning up" << m_pixelBuffer->getSizeInBytes() / 1024.0f << "KB of CPU copy of pixel data" << logdebug;
    */

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

bool TextureCubeFSLoader::decode(shared_ptr<FileDataSource> source)
{
    std::string buffer(source->getSizeInBytes(), 0);
    source->getRaw(&buffer[0], buffer.size());

    auto jsonRoot = utils::json::fromSource(buffer);
    if (jsonRoot.empty())
        return false;

    auto srcPathDir = FileSystemHelpers::getFileDirectory(source->getPath());

    auto getSource = [&srcPathDir](const std::string &texturePath)
    {
        auto fullPath = FileSystemHelpers::pathConcatenate(srcPathDir, texturePath);
        return svc().fileSystem().openFile(fullPath);
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
            glog << "Failed to decode cube texture. Image(s) invalid." << logwarn;
            return;
        }
    }

    auto texture = static_cast<Texture*>(resource);
    auto descr = svc().renderManager().getBackend().createTextureCube(m_pixelBuffers, m_params);
    if (descr.valid())
    {
        texture->setDescriptor(descr);

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

unique_ptr<PixelBuffer> GetPixelBufferFromSource(shared_ptr<FileDataSource> source, bool forceRgba)
{
    return LoadPixelBuffer(source, forceRgba);
}

}
