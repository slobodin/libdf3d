#include "TextureCube.h"

#include <base/Service.h>
#include "RenderManager.h"
#include "RendererBackend.h"
#include "Texture2D.h"

namespace df3d { namespace render {

static const std::map<CubeFace, GLenum> MapSidesToGl = 
{
    { CUBE_FACE_POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X },
    { CUBE_FACE_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X },
    { CUBE_FACE_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y },
    { CUBE_FACE_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y },
    { CUBE_FACE_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z },
    { CUBE_FACE_NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z }
};

bool TextureCube::createGLTexture(unique_ptr<PixelBuffer> images[CUBE_FACES_COUNT])
{
    glGenTextures(1, &m_glid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    setupGlWrapMode(GL_TEXTURE_CUBE_MAP, m_params.getWrapMode());
    setupGlTextureFiltering(GL_TEXTURE_CUBE_MAP, m_params.getFiltering(), m_params.isMipmapped());

    for (int i = 0; i < CUBE_FACES_COUNT; i++)
    {
        GLint glPixelFormat = 0;
        switch (images[i]->getFormat())
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

        auto data = images[i]->getData();
        auto width = images[i]->getWidth();
        auto height = images[i]->getHeight();
        glTexImage2D(MapSidesToGl.find((CubeFace)i)->second, 0, glPixelFormat, width, height, 0, glPixelFormat, GL_UNSIGNED_BYTE, data);
    }

    if (m_params.isMipmapped())
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

TextureCube::TextureCube(TextureCreationParams params)
    : Texture(params)
{

}

TextureCube::~TextureCube()
{
    deleteGLTexture();
}

bool TextureCube::bind(size_t unit)
{
    if (!isInitialized())
        return false;

    assert(m_glid);

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glid);

    return true;
}

void TextureCube::unbind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

} }
