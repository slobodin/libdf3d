#pragma once

#include <resources/Resource.h>
#include <render/Texture2D.h>

namespace df3d { namespace resources {

class Texture2DManualLoader : public ManualResourceLoader
{
    unique_ptr<render::PixelBuffer> m_pixelBuffer;
    render::TextureCreationParams m_params;

public:
    Texture2DManualLoader(unique_ptr<render::PixelBuffer> pixelBuffer, render::TextureCreationParams params);

    render::Texture2D* load() override;
};

class Texture2DFSLoader : public FSResourceLoader
{
    std::string m_pathToTexture;
    render::TextureCreationParams m_params;

    unique_ptr<render::PixelBuffer> m_pixelBuffer;

public:
    Texture2DFSLoader(const std::string &path, const render::TextureCreationParams &params, ResourceLoadingMode lm);

    render::Texture2D* createDummy(const ResourceGUID &guid) override;
    void decode(shared_ptr<FileDataSource> source) override;
    void onDecoded(Resource *resource) override;
};

} }
