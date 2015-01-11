#pragma once

#include <CEGUI/CEGUI.h>

FWD_MODULE_CLASS(render, Texture)
FWD_MODULE_CLASS(render, Texture2D)

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiGeometryBufferImpl;
class CeguiTextureTargetImpl;
class CeguiTextureImpl;

class CeguiRendererImpl : public CEGUI::Renderer
{
    const CEGUI::String m_rendererId;

    CEGUI::Sizef m_displaySize;
    const CEGUI::Vector2f m_displayDpi = { 96.0f, 96.0f };

    CEGUI::RenderTarget *m_defaultRenderTarget;
    CEGUI::RenderTarget *m_activeRenderTarget = nullptr;
    std::vector<CeguiGeometryBufferImpl*> m_geometryBuffers;
    std::vector<CeguiTextureTargetImpl*> m_textureTargets;

    typedef std::map<CEGUI::String, CeguiTextureImpl*, CEGUI::StringFastLessCompare CEGUI_MAP_ALLOC(CEGUI::String, CeguiTextureImpl*)> TextureMap;
    TextureMap m_textures;

    CeguiRendererImpl(int width, int height);
    virtual ~CeguiRendererImpl();

    void logTextureCreation(const CEGUI::String &name);
    void logTextureDestruction(const CEGUI::String &name);

public:
    static CeguiRendererImpl& bootstrapSystem(int width, int height, const int abi = CEGUI_VERSION_ABI);
    static void destroySystem();

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
    CEGUI::Texture& createTexture(const CEGUI::String &name, shared_ptr<render::Texture2D> texture);
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

    void setActiveRenderTarget(CEGUI::RenderTarget *target);
    const CEGUI::RenderTarget* getActiveRenderTarget() const;
};

} } }
