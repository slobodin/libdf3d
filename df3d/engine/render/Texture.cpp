#include "Texture.h"

#include <df3d/engine/EngineController.h>
#include "RenderManager.h"
#include "IRenderBackend.h"

namespace df3d {

Texture::Texture(TextureHandle handle, size_t width, size_t height)
    : m_handle(handle),
    m_width(width),
    m_height(height)
{

}

Texture::~Texture()
{
    if (m_handle.isValid())
        svc().renderManager().getBackend().destroyTexture(m_handle);
}

TextureHandle Texture::getHandle() const
{
    return m_handle;
}

void Texture::setHandle(TextureHandle handle)
{
    DF3D_ASSERT(handle.isValid());

    if (m_handle.isValid())
    {
        DFLOG_WARN("Texture already has a handle");
        return;
    }

    m_handle = handle;
}

}
