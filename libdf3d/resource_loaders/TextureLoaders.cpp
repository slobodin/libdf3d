#include "TextureLoaders.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/render/RenderManager.h>
#include <libdf3d/render/IRenderBackend.h>
#include <libdf3d/io/FileSystem.h>
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

static unique_ptr<PixelBuffer> loadPixelBuffer(shared_ptr<FileDataSource> source)
{
    if (!source)
    {
        glog << "Failed to load pixel buffer from file source. Source is null." << logwarn;
        return nullptr;
    }

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
    else if (bpp == STBI_grey)
        fmt = PixelFormat::GRAYSCALE;
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
    auto descr = svc().renderManager().getBackend().createTexture2D(*m_pixelBuffer, m_params);
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
    m_pixelBuffer = loadPixelBuffer(source);
    return m_pixelBuffer != nullptr;
}

void Texture2DFSLoader::onDecoded(Resource *resource)
{
    auto texture = static_cast<Texture*>(resource);

    auto descr = svc().renderManager().getBackend().createTexture2D(*m_pixelBuffer, m_params);
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
    source->getRaw(&buffer[0], source->getSizeInBytes());

    auto jsonRoot = utils::json::fromSource(buffer);
    if (jsonRoot.empty())
        return false;

    auto srcPathDir = svc().fileSystem().getFileDirectory(source->getPath());

    auto getSource = [&srcPathDir](const std::string &texturePath)
    {
        auto fullPath = svc().fileSystem().pathConcatenate(srcPathDir, texturePath);
        return svc().fileSystem().openFile(fullPath);
    };

    m_pixelBuffers[(size_t)CubeFace::POSITIVE_X] = loadPixelBuffer(getSource(jsonRoot["positive_x"].asString()));
    m_pixelBuffers[(size_t)CubeFace::NEGATIVE_X] = loadPixelBuffer(getSource(jsonRoot["negative_x"].asString()));
    m_pixelBuffers[(size_t)CubeFace::POSITIVE_Y] = loadPixelBuffer(getSource(jsonRoot["positive_y"].asString()));
    m_pixelBuffers[(size_t)CubeFace::NEGATIVE_Y] = loadPixelBuffer(getSource(jsonRoot["negative_y"].asString()));
    m_pixelBuffers[(size_t)CubeFace::POSITIVE_Z] = loadPixelBuffer(getSource(jsonRoot["positive_z"].asString()));
    m_pixelBuffers[(size_t)CubeFace::NEGATIVE_Z] = loadPixelBuffer(getSource(jsonRoot["negative_z"].asString()));

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

}
