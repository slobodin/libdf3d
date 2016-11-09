#include "TextureResource.h"

#include "loaders/TextureLoader_stbi.h"
#include "loaders/TextureLoader_webp.h"
#include "loaders/TextureLoader_pvrtc.h"
#include "ResourceFileSystem.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/lib/JsonUtils.h>

namespace df3d {

static df3d::TextureResourceData* LoadTextureDataFromFile(const std::string &path, Allocator &allocator, bool forceRgba = false)
{
    auto &fs = svc().resourceManager().getFS();

    auto textureSource = fs.open(path.c_str());
    if (!textureSource)
        return nullptr;

    df3d::TextureResourceData *result = nullptr;
    const auto ext = FileSystemHelpers::getFileExtension(path);
    if (ext == ".webp")
        result = TextureLoader_webp(*textureSource, allocator, forceRgba);
    else if (ext == ".pvr")
        result = TextureLoader_pvrtc(*textureSource, allocator);
    else
        result = TextureLoader_stbi(*textureSource, allocator, forceRgba);

    fs.close(textureSource);

    return result;
}

static uint32_t GetTextureFlags(const Json::Value &root)
{
    uint32_t retRes = 0;

    if (root.isMember("filtering"))
    {
        auto valueStr = root["filtering"].asString();
        if (valueStr == "NEAREST")
            retRes |= TEXTURE_FILTERING_NEAREST;
        else if (valueStr == "BILINEAR")
            retRes |= TEXTURE_FILTERING_BILINEAR;
        else if (valueStr == "TRILINEAR")
            retRes |= TEXTURE_FILTERING_TRILINEAR;
        else if (valueStr == "ANISOTROPIC")
            retRes |= TEXTURE_FILTERING_ANISOTROPIC;
        else
            DFLOG_WARN("Unknown filtering mode %s", valueStr.c_str());
    }
    else
    {
        retRes |= TEXTURE_FILTERING_ANISOTROPIC;
    }

    if (root.isMember("wrap_mode"))
    {
        auto valueStr = root["wrap_mode"].asString();
        if (valueStr == "WRAP")
            retRes |= TEXTURE_WRAP_MODE_REPEAT;
        else if (valueStr == "CLAMP")
            retRes |= TEXTURE_WRAP_MODE_CLAMP;
        else
            DFLOG_WARN("Unknown wrap_mode mode %s", valueStr.c_str());
    }
    else
    {
        retRes |= TEXTURE_WRAP_MODE_REPEAT;
    }

    return retRes;
}

bool TextureHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    if (!root.isMember("path"))
        return false;

    m_flags = GetTextureFlags(root);

    m_resourceData = LoadTextureDataFromFile(root["path"].asString(), allocator);

    return m_resourceData != nullptr;
}

void TextureHolder::decodeCleanup(Allocator &allocator)
{
    MAKE_DELETE(allocator, m_resourceData);
    m_resourceData = nullptr;
}

bool TextureHolder::createResource(Allocator &allocator)
{
    auto &backend = svc().renderManager().getBackend();

    auto handle = backend.createTexture2D(m_resourceData->info,
                                          m_flags,
                                          m_resourceData->pixels.data(),
                                          m_resourceData->pixels.size());
    if (!handle.isValid())
        return false;

    m_resource = MAKE_NEW(allocator, TextureResource)();
    m_resource->handle = handle;
    m_resource->width = m_resourceData->info.width;
    m_resource->height = m_resourceData->info.height;
    return true;
}

void TextureHolder::destroyResource(Allocator &allocator)
{
    DF3D_ASSERT(m_resource->handle.isValid());
    svc().renderManager().getBackend().destroyTexture(m_resource->handle);
    MAKE_DELETE(allocator, m_resource);
    m_resource = nullptr;
}

TextureResourceData* LoadTexture_Workaround(ResourceDataSource &dataSource, Allocator &alloc)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return nullptr;

    if (!root.isMember("path"))
        return nullptr;

    return LoadTextureDataFromFile(root["path"].asString(), alloc, true);
}

}
