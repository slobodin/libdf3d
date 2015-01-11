#pragma once

#include "OpenGLCommon.h"

namespace df3d { namespace render {

extern const DF3D_DLL int ANISOTROPY_LEVEL_MAX;

class Texture
{
public:
    enum class Filtering
    {
        NEAREST,
        BILINEAR,
        TRILINEAR
    };

    enum class WrapMode
    {
        WRAP,
        CLAMP
    };

protected:
    Filtering m_filteringMode = Filtering::TRILINEAR;
    WrapMode m_wrapMode = WrapMode::CLAMP;
    bool m_mipmapped = true;
    int m_maxAnisotropy = 1;

    // GL data.
    GLuint m_glid = 0;

    bool isPot(size_t v);
    size_t getNextPot(size_t v);
    static GLint getGlFilteringMode(Filtering filtering, bool mipmapped);
    static GLint getGlWrapMode(WrapMode mode);

public:
    Texture();
    virtual ~Texture();

    Filtering filtering() const { return m_filteringMode; }
    bool isMipmapped() const { return m_mipmapped; }
    WrapMode wrapMode() const { return m_wrapMode; }

    unsigned getGLId() const { return m_glid; }

    void setFilteringMode(Filtering newFiltering);
    void setMipmapped(bool hasMipmaps);
    void setWrapMode(WrapMode mode);
    void setMaxAnisotropy(int aniso);

    virtual bool bind(size_t unit) = 0;
    virtual void unbind() = 0;
};

} }