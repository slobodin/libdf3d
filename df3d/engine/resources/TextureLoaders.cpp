#include "TextureLoaders.h"

#include "loaders/TextureLoader_stbi.h"
#include "loaders/TextureLoader_webp.h"
#include "loaders/TextureLoader_pvrtc.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/io/DataSource.h>

namespace df3d {

static bool LoadPixelBuffer(shared_ptr<DataSource> source, bool forceRGBA, PixelData &outPixelData)
{
    const auto ext = FileSystemHelpers::getFileExtension(source->getPath());
    if (ext == ".webp")
    {
        return TextureLoader_webp::load(source, forceRGBA, outPixelData);
    }
    else if (ext == ".pvr")
    {
        DF3D_ASSERT_MESS(forceRGBA == false, "forceRGBA is not supported for PVRTC");
        return TextureLoader_pvrtc::load(source, outPixelData);
    }
    else
    {
        return TextureLoader_stbi::load(source, forceRGBA, outPixelData);
    }
}

PixelData::PixelData()
    : data(MemoryManager::allocDefault())
{

}

Texture2DManualLoader::Texture2DManualLoader(const TextureInfo &info, const void *data, size_t dataSize)
    : m_info(info),
    m_data(data),
    m_dataSize(dataSize)
{

}

Texture* Texture2DManualLoader::load()
{
    auto handle = svc().renderManager().getBackend().createTexture2D(m_info, m_data, m_dataSize);
    if (!handle.isValid())
        return nullptr;

    return new Texture(handle, m_info.width, m_info.height);
}

Texture2DFSLoader::Texture2DFSLoader(const std::string &path, uint32_t flags, ResourceLoadingMode lm)
    : FSResourceLoader(lm),
    m_pathToTexture(path)
{
    m_data.info.flags = flags;
}

Texture* Texture2DFSLoader::createDummy()
{
    return new Texture();
}

bool Texture2DFSLoader::decode(shared_ptr<DataSource> source)
{
    return LoadPixelBuffer(source, false, m_data);
}

void Texture2DFSLoader::onDecoded(Resource *resource)
{
    auto texture = static_cast<Texture*>(resource);

    auto handle = svc().renderManager().getBackend().createTexture2D(m_data.info, &m_data.data[0], m_data.data.size());
    if (handle.isValid())
    {
        texture->setHandle(handle);
        texture->setWidthAndHeight(m_data.info.width, m_data.info.height);
    }

    m_data.data.clear();
}

bool GetPixelBufferFromSource(shared_ptr<DataSource> source, bool forceRGBA, PixelData &outPixelData)
{
    return LoadPixelBuffer(source, forceRGBA, outPixelData);
}

}
