#pragma once

#include <CEGUI/CEGUI.h>

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiRendererImpl : public CEGUI::Renderer
{
public:
    static CeguiRendererImpl& bootstrapSystem(const int abi = CEGUI_VERSION_ABI);
    static void destroySystem();

    static CeguiRendererImpl& create(const int abi = CEGUI_VERSION_ABI);
    static void destroy(CeguiRendererImpl &renderer);

    CEGUI::RenderTarget& getDefaultRenderTarget();
    CEGUI::GeometryBuffer& createGeometryBuffer();
    void destroyGeometryBuffer(const CEGUI::GeometryBuffer &buffer);
    void destroyAllGeometryBuffers();
    CEGUI::TextureTarget* createTextureTarget();
    void destroyTextureTarget(CEGUI::TextureTarget *target);
    void destroyAllTextureTargets();
    CEGUI::Texture& createTexture(const CEGUI::String &name);
    CEGUI::Texture& createTexture(const CEGUI::String &name, const CEGUI::String &filename, const CEGUI::String &resourceGroup);
    CEGUI::Texture& createTexture(const CEGUI::String &name, const CEGUI::Sizef &size);
    void destroyTexture(CEGUI::Texture &texture);
    void destroyTexture(const CEGUI::String &name);
    void destroyAllTextures();
    CEGUI::Texture& getTexture(const CEGUI::String &name) const;
    bool isTextureDefined(const CEGUI::String &name) const;
    void beginRendering();
    void endRendering();
    void setDisplaySize(const CEGUI::Sizef &sz);
    const CEGUI::Sizef& getDisplaySize() const;
    const CEGUI::Vector2f& getDisplayDPI() const;
    CEGUI::uint getMaxTextureSize() const;
    const CEGUI::String& getIdentifierString() const;

protected:
    CeguiRendererImpl();
    virtual ~CeguiRendererImpl();
};

} } }
