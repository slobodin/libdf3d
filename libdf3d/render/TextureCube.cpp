#include "df3d_pch.h"
#include "TextureCube.h"

#include "Image.h"

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
        if (!m_images[i] || !m_images[i]->valid())
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

    setupGlWrapMode(GL_TEXTURE_CUBE_MAP, m_wrapMode);
    setupGlTextureFiltering(GL_TEXTURE_CUBE_MAP, m_filteringMode, m_mipmapped);

    for (int i = 0; i < 6; i++)
    {
        GLint glPixelFormat = 0;
        switch (m_images[i]->pixelFormat())
        {
        case Image::Format::RGB:
        case Image::Format::BGR:
            glPixelFormat = GL_RGB;
            break;
        case Image::Format::RGBA:
            glPixelFormat = GL_RGBA;
            break;
        default:
            base::glog << "Invalid GL texture pixel format" << base::logwarn;
            return false;
        }

        auto data = m_images[i]->data();
        auto width = m_images[i]->width();
        auto height = m_images[i]->height();
        glTexImage2D(MapSidesToGl[i], 0, glPixelFormat, width, height, 0, glPixelFormat, GL_UNSIGNED_BYTE, data);
    }

    if (m_mipmapped)
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

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

TextureCube::TextureCube(shared_ptr<Image> positiveX, shared_ptr<Image> negativeX,
    shared_ptr<Image> positiveY, shared_ptr<Image> negativeY,
    shared_ptr<Image> positiveZ, shared_ptr<Image> negativeZ)
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