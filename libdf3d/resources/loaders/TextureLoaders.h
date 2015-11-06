#pragma once

#include <resources/Resource.h>
#include <render/Texture2D.h>
#include <render/TextureCube.h>

namespace df3d {

class Texture2DManualLoader : public ManualResourceLoader
{
    unique_ptr<PixelBuffer> m_pixelBuffer;
    TextureCreationParams m_params;

public:
    Texture2DManualLoader(unique_ptr<PixelBuffer> pixelBuffer, TextureCreationParams params);

    Texture2D* load() override;
};

class Texture2DFSLoader : public FSResourceLoader
{
    std::string m_pathToTexture;
    TextureCreationParams m_params;

    unique_ptr<PixelBuffer> m_pixelBuffer;

public:
    Texture2DFSLoader(const std::string &path, const TextureCreationParams &params, ResourceLoadingMode lm);

    Texture2D* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

class TextureCubeFSLoader : public FSResourceLoader
{
    std::string m_jsonPath;
    TextureCreationParams m_params;

    unique_ptr<PixelBuffer> m_pixelBuffers[(int)CubeFace::COUNT];

public:
    TextureCubeFSLoader(const std::string &path, const TextureCreationParams &params, ResourceLoadingMode lm);

    TextureCube* createDummy() override;
    bool decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

}
