#pragma once

#include <CEGUI/Texture.h>

FWD_MODULE_CLASS(render, Texture)

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiTextureImpl : public CEGUI::Texture
{
    const CEGUI::String m_name;
    CEGUI::Sizef m_originalDataSize = { 0.0f, 0.0f };
    CEGUI::Sizef m_dataSize = { 0.0f, 0.0f };
    CEGUI::Vector2f m_texelScaling = { 0.0f, 0.0f };

    shared_ptr<render::Texture> m_texture;
    void updateSizes();
    void updateCachedScaleValues();

public:
    CeguiTextureImpl(const CEGUI::String &name);
    CeguiTextureImpl(const CEGUI::String &name, const CEGUI::String &filename, const CEGUI::String &resourceGroup);
    CeguiTextureImpl(const CEGUI::String &name, const CEGUI::Sizef &size);
    CeguiTextureImpl(const CEGUI::String &name, shared_ptr<render::Texture> texture);

    const CEGUI::String& getName() const;
    const CEGUI::Sizef& getSize() const;
    const CEGUI::Sizef& getOriginalDataSize() const;
    const CEGUI::Vector2f& getTexelScaling() const;
    void loadFromFile(const CEGUI::String &filename, const CEGUI::String &resourceGroup);
    void loadFromMemory(const void *buffer, const CEGUI::Sizef &buffer_size, PixelFormat pixel_format);
    void blitFromMemory(const void *sourceData, const CEGUI::Rectf &area);
    void blitToMemory(void *targetData);
    bool isPixelFormatSupported(const PixelFormat fmt) const;

    shared_ptr<render::Texture> getDf3dTexture() const;
};

} } }