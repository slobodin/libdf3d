#include "TextureCube.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/base/FrameStats.h>
#include "RenderManager.h"
#include "RendererBackend.h"

namespace df3d {

static const std::map<CubeFace, GLenum> MapSidesToGl = 
{
    { CubeFace::POSITIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X },
    { CubeFace::NEGATIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X },
    { CubeFace::POSITIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y },
    { CubeFace::NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y },
    { CubeFace::POSITIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z },
    { CubeFace::NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z }
};

bool TextureCube::createGLTexture(unique_ptr<PixelBuffer> images[(size_t)CubeFace::COUNT])
{
    m_sizeInBytes = 0;

    glGenTextures(1, &m_glid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glid);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    setupGlWrapMode(GL_TEXTURE_CUBE_MAP, m_params.getWrapMode());
    setupGlTextureFiltering(GL_TEXTURE_CUBE_MAP, m_params.getFiltering(), m_params.isMipmapped());

    for (int i = 0; i < (size_t)CubeFace::COUNT; i++)
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
            glog << "Invalid GL texture pixel format" << logwarn;
            return false;
        }

        auto data = images[i]->getData();
        auto width = images[i]->getWidth();
        auto height = images[i]->getHeight();
        glTexImage2D(MapSidesToGl.find((CubeFace)i)->second, 0, glPixelFormat, width, height, 0, glPixelFormat, GL_UNSIGNED_BYTE, data);

        m_sizeInBytes += images[i]->getSizeInBytes();
    }

    if (m_params.isMipmapped())
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    svc().getFrameStats().addTexture(*this);

    printOpenGLError();

    return true;
}

void TextureCube::deleteGLTexture()
{
    if (m_glid)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glDeleteTextures(1, &m_glid);

        svc().getFrameStats().removeTexture(*this);

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

bool TextureCube::bind()
{
    if (!isInitialized())
        return false;

    assert(m_glid);

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_glid);

    return true;
}

void TextureCube::unbind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

size_t TextureCube::getSizeInBytes() const
{
    return m_sizeInBytes;
}

}
