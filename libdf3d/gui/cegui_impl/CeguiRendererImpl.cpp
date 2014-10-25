#include "df3d_pch.h"

#include "CeguiRendererImpl.h"
#include "CeguiResourceProviderImpl.h"
#include "CeguiImageCodecImpl.h"
#include "CeguiViewportTargetImpl.h"
#include "CeguiGeometryBufferImpl.h"
#include "CeguiTextureTargetImpl.h"

#include <base/Controller.h>
#include <render/RenderManager.h>
#include <render/Renderer.h>

namespace df3d { namespace gui { namespace cegui_impl {

using namespace CEGUI;

CeguiRendererImpl::CeguiRendererImpl(int width, int height)
    : m_rendererId("CEGUI libdf3d renderer."),
    m_displaySize((float)width, (float)height),
    m_displayDpi(96.0f, 96.0f)
{
    m_defaultRenderTarget = CEGUI_NEW_AO CeguiViewportTargetImpl(*this, width, height);
}

CeguiRendererImpl::~CeguiRendererImpl()
{
    CEGUI_DELETE_AO m_defaultRenderTarget;
}

CeguiRendererImpl& CeguiRendererImpl::bootstrapSystem(int width, int height, const int abi)
{
    System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);

    if (System::getSingletonPtr())
        CEGUI_THROW(InvalidRequestException("CEGUI::System object is already initialised."));

    auto &renderer = *CEGUI_NEW_AO CeguiRendererImpl(width, height);
    auto &rp = *CEGUI_NEW_AO CeguiResourceProviderImpl();
    auto &ic = *CEGUI_NEW_AO CeguiImageCodecImpl();

    System::create(renderer, &rp, nullptr, &ic);

    return renderer;
}

void CeguiRendererImpl::destroySystem()
{
    System *sys;
    if (!(sys = System::getSingletonPtr()))
        CEGUI_THROW(InvalidRequestException("CEGUI::System object is not created or was already destroyed."));

    auto renderer = static_cast<CeguiRendererImpl*>(sys->getRenderer());
    auto rp = static_cast<CeguiResourceProviderImpl*>(sys->getResourceProvider());
    auto ic = &static_cast<CeguiImageCodecImpl&>(sys->getImageCodec());

    System::destroy();
    CEGUI_DELETE_AO ic;
    CEGUI_DELETE_AO rp;
    CEGUI_DELETE_AO renderer;
}

CEGUI::RenderTarget& CeguiRendererImpl::getDefaultRenderTarget()
{
    return *m_defaultRenderTarget;
}

CEGUI::GeometryBuffer& CeguiRendererImpl::createGeometryBuffer()
{
    auto geometryBuffer = CEGUI_NEW_AO CeguiGeometryBufferImpl();
    m_geometryBuffers.push_back(geometryBuffer);

    return *geometryBuffer;
}

void CeguiRendererImpl::destroyGeometryBuffer(const CEGUI::GeometryBuffer &buffer)
{
    auto found = std::find(m_geometryBuffers.begin(), m_geometryBuffers.end(), &buffer);
    if (found != m_geometryBuffers.end())
    {
        m_geometryBuffers.erase(found);
        CEGUI_DELETE_AO &buffer;
    }
}

void CeguiRendererImpl::destroyAllGeometryBuffers()
{
    while (!m_geometryBuffers.empty())
        destroyGeometryBuffer(**m_geometryBuffers.begin());
}

CEGUI::TextureTarget* CeguiRendererImpl::createTextureTarget()
{
    auto textureTarget = CEGUI_NEW_AO CeguiTextureTargetImpl(*this);
    m_textureTargets.push_back(textureTarget);

    return textureTarget;
}

void CeguiRendererImpl::destroyTextureTarget(CEGUI::TextureTarget *target)
{
    auto found = std::find(m_textureTargets.begin(), m_textureTargets.end(), target);
    if (found != m_textureTargets.end())
    {
        m_textureTargets.erase(found);
        CEGUI_DELETE_AO target;
    }
}

void CeguiRendererImpl::destroyAllTextureTargets()
{
    while (!m_textureTargets.empty())
        destroyTextureTarget(*m_textureTargets.begin());
}

CEGUI::Texture& CeguiRendererImpl::createTexture(const CEGUI::String &name)
{

}

CEGUI::Texture& CeguiRendererImpl::createTexture(const CEGUI::String &name, const CEGUI::String &filename, const CEGUI::String &resourceGroup)
{

}

CEGUI::Texture& CeguiRendererImpl::createTexture(const CEGUI::String &name, const CEGUI::Sizef &size)
{

}

void CeguiRendererImpl::destroyTexture(CEGUI::Texture &texture)
{

}

void CeguiRendererImpl::destroyTexture(const CEGUI::String &name)
{

}

void CeguiRendererImpl::destroyAllTextures()
{

}

CEGUI::Texture& CeguiRendererImpl::getTexture(const CEGUI::String &name) const
{

}

bool CeguiRendererImpl::isTextureDefined(const CEGUI::String &name) const
{

}

void CeguiRendererImpl::beginRendering()
{

}

void CeguiRendererImpl::endRendering()
{

}

void CeguiRendererImpl::setDisplaySize(const CEGUI::Sizef &sz)
{
    m_displaySize = sz;
}

const CEGUI::Sizef& CeguiRendererImpl::getDisplaySize() const
{
    return m_displaySize;
}

const CEGUI::Vector2f& CeguiRendererImpl::getDisplayDPI() const
{
    return m_displayDpi;
}

CEGUI::uint CeguiRendererImpl::getMaxTextureSize() const
{
    return g_renderManager->getRenderer()->getMaxTextureSize();
}

const CEGUI::String& CeguiRendererImpl::getIdentifierString() const
{
    return m_rendererId;
}

} } }