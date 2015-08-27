#include "df3d_pch.h"
#include "TextureCube.h"

#include <base/SystemsMacro.h>
#include "RenderManager.h"
#include "RendererBackend.h"
#include "Texture2D.h"

namespace df3d { namespace render {

static const std::vector<GLenum> MapSidesToGl = 
{
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

bool TextureCube::imagesValid() const
{
    for (int i = 0; i < 6; i++)
        if (!m_images[i] || !m_images[i]->isValid())
            return false;

    return true;
}

bool TextureCube::createGLTexture()
{
    if (m_glid)
        return true;

    glGenTextures(1, &m_glid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    const auto &defaultCaps = g_renderManager->getRenderingCapabilities();
    if (!m_filtering)
        m_filtering = defaultCaps.textureFiltering;
    if (!m_mipmapped)
        m_mipmapped = defaultCaps.mipmaps;

    setupGlWrapMode(GL_TEXTURE_CUBE_MAP, m_wrapMode);
    setupGlTextureFiltering(GL_TEXTURE_CUBE_MAP, getFilteringMode(), isMipmapped());

    for (int i = 0; i < 6; i++)
    {
        GLint glPixelFormat = 0;
        switch (m_images[i]->getPixelFormat())
        {
        case PixelFormat::RGB:
        case PixelFormat::BGR:
            glPixelFormat = GL_RGB;
            break;
        case PixelFormat::RGBA:
            glPixelFormat = GL_RGBA;
            break;
        default:
            base::glog << "Invalid GL texture pixel format" << base::logwarn;
            return false;
        }

        auto data = m_images[i]->getPixelBufferData();
        auto width = m_images[i]->getOriginalWidth();
        auto height = m_images[i]->getOriginalHeight();
        glTexImage2D(MapSidesToGl[i], 0, glPixelFormat, width, height, 0, glPixelFormat, GL_UNSIGNED_BYTE, data);
    }

    if (isMipmapped())
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    for (auto i = 0; i < 6; i++)
    {
        g_resourceManager->unloadResource(m_images[i]);
        m_images[i].reset();
    }

    printOpenGLError();

    return true;
}

void TextureCube::deleteGLTexture()
{
    if (m_glid)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDeleteTextures(1, &m_glid);

        m_glid = 0;
    }
}

TextureCube::TextureCube(shared_ptr<Texture2D> positiveX, shared_ptr<Texture2D> negativeX,
                         shared_ptr<Texture2D> positiveY, shared_ptr<Texture2D> negativeY,
                         shared_ptr<Texture2D> positiveZ, shared_ptr<Texture2D> negativeZ)
{
    m_images[0] = positiveX;
    m_images[1] = negativeX;
    m_images[2] = positiveY;
    m_images[3] = negativeY;
    m_images[4] = positiveZ;
    m_images[5] = negativeZ;
}

TextureCube::~TextureCube()
{
    deleteGLTexture();
}

bool TextureCube::bind(size_t unit)
{
    if (!imagesValid())
        return false;

    if (createGLTexture())
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_glid);
        return true;
    }
    else
        return false;
}

void TextureCube::unbind()
{
    if (!m_glid)
        return;

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

} }
