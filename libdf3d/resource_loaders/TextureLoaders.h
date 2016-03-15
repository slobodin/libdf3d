#pragma once

#include <libdf3d/resources/Resource.h>
#include <libdf3d/render/Texture.h>

namespace df3d {

class Texture2DManualLoader : public ManualResourceLoader
{
    unique_ptr<PixelBuffer> m_pixelBuffer;
    TextureCreationParams m_params;

public:
    Texture2DManualLoader(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params);

    Texture* load() override;
};

class Texture2DFSLoader : public FSResourceLoader
{
    std::string m_pathToTexture;
    TextureCreationParams m_params;

    unique_ptr<PixelBuffer> m_pixelBuffer;

public:
    Texture2DFSLoader(const std::string &path, const TextureCreationParams &params, ResourceLoadingMode lm);

    Texture* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

class TextureCubeFSLoader : public FSResourceLoader
{
    std::string m_jsonPath;
    TextureCreationParams m_params;

    unique_ptr<PixelBuffer> m_pixelBuffers[(size_t)CubeFace::COUNT];

public:
    TextureCubeFSLoader(const std::string &path, const TextureCreationParams &params, ResourceLoadingMode lm);

    Texture* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

unique_ptr<PixelBuffer> GetPixelBufferFromSource(shared_ptr<FileDataSource> source, int reqComp);

}
