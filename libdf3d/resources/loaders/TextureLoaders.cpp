#include "TextureLoaders.h"

#include <base/Service.h>
#include <resources/FileDataSource.h>
#include <utils/JsonUtils.h>

#define STB_IMAGE_IMPLEMENTATION
#ifndef STB_DO_ERROR_PRINT
#define STBI_NO_FAILURE_STRINGS     // not thread-safe
#endif
#include <stb/stb_image.h>

namespace df3d {

// stb image loader helpers.
namespace
{
    // fill 'data' with 'size' bytes.  return number of bytes actually read
    int read(void *user, char *data, int size)
    {
        auto dataSource = static_cast<FileDataSource*>(user);
        return dataSource->getRaw(data, size);
    }

    // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
    void skip(void *user, int n)
    {
        if (n < 0)
        {
            // TODO:
            assert(false);
        }

        auto dataSource = static_cast<FileDataSource*>(user);
        dataSource->seek(n, std::ios_base::cur);
    }

    // returns nonzero if we are at end of file/data
    int eof(void *user)
    {
        auto dataSource = static_cast<FileDataSource*>(user);
        return dataSource->tell() >= dataSource->getSize();
    }

    unique_ptr<PixelBuffer> loadPixelBuffer(shared_ptr<FileDataSource> source)
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
}

Texture2DManualLoader::Texture2DManualLoader(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params)
    : m_pixelBuffer(std::move(pixelBuffer)), m_params(params)
{

}

Texture2D* Texture2DManualLoader::load()
{
    return new Texture2D(*m_pixelBuffer, m_params);
}

Texture2DFSLoader::Texture2DFSLoader(const std::string &path, const TextureCreationParams &params, ResourceLoadingMode lm)
    : FSResourceLoader(lm),
    m_pathToTexture(path),
    m_params(params)
{

}

Texture2D* Texture2DFSLoader::createDummy()
{
    return new Texture2D(m_params);
}

bool Texture2DFSLoader::decode(shared_ptr<FileDataSource> source)
{
    m_pixelBuffer = loadPixelBuffer(source);
    return m_pixelBuffer != nullptr;
}

void Texture2DFSLoader::onDecoded(Resource *resource)
{
    auto texture = static_cast<Texture2D*>(resource);
    texture->createGLTexture(*m_pixelBuffer);

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

TextureCube* TextureCubeFSLoader::createDummy()
{
    return new TextureCube(m_params);
}

bool TextureCubeFSLoader::decode(shared_ptr<FileDataSource> source)
{
    std::string buffer(source->getSize(), 0);
    source->getRaw(&buffer[0], source->getSize());

    auto jsonRoot = utils::json::fromFile(buffer);
    if (jsonRoot.empty())
        return false;

    auto srcPathDir = svc().filesystem.getFileDirectory(source->getPath());

    auto getSource = [&srcPathDir](const std::string &texturePath)
    {
        auto fullPath = svc().filesystem.pathConcatenate(srcPathDir, texturePath);
        return svc().filesystem.openFile(fullPath);
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

    auto texture = static_cast<TextureCube*>(resource);

    texture->createGLTexture(m_pixelBuffers);

    // Clean up main memory.
    for (int i = 0; i < (size_t)CubeFace::COUNT; i++)
        m_pixelBuffers[i].reset();
}

}
