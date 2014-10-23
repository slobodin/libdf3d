#pragma once

#include <CEGUI/Texture.h>

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiTextureImpl : public CEGUI::Texture
{
public:
    const CEGUI::String& getName() const;
    const CEGUI::Sizef& getSize() const;
    const CEGUI::Sizef& getOriginalDataSize() const;
    const CEGUI::Vector2f& getTexelScaling() const;
    void loadFromFile(const CEGUI::String &filename, const CEGUI::String &resourceGroup);
    void loadFromMemory(const void *buffer, const CEGUI::Sizef &buffer_size, PixelFormat pixel_format);
    void blitFromMemory(const void *sourceData, const CEGUI::Rectf &area);
    void blitToMemory(void *targetData);
    bool isPixelFormatSupported(const PixelFormat fmt) const;
};

} } }