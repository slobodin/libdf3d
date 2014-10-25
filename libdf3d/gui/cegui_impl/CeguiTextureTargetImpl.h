#pragma once

#include <CEGUI/TextureTarget.h>
#include "CeguiRenderTargetImpl.h"

namespace df3d { namespace gui { namespace cegui_impl {

class CeguiTextureImpl;

class CeguiTextureTargetImpl : public CeguiRenderTargetImpl<CEGUI::TextureTarget>
{
    CeguiTextureImpl *m_texture = nullptr;

public:
    CeguiTextureTargetImpl(CeguiRendererImpl &owner);
    ~CeguiTextureTargetImpl();

    bool isImageryCache() const;
    void clear();
    CEGUI::Texture& getTexture() const;
    void declareRenderSize(const CEGUI::Sizef &sz);
    bool isRenderingInverted() const;
};

} } }
