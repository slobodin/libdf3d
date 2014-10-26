#pragma once

namespace df3d { namespace render {

class Image;

class Texture
{
public:
    enum Filtering
    {
        NEAREST,
        BILINEAR,
        TRILINEAR
    };

    enum Type
    {
        TEXTURE_TYPE_NONE,
        TEXTURE_1D,
        TEXTURE_2D,
        TEXTURE_3D,
        TEXTURE_CUBE
    };

    enum WrapMode
    {
        WM_WRAP,
        WM_CLAMP
    };

private:
    Filtering m_filteringMode = TRILINEAR;
    Type m_textureType = TEXTURE_2D;
    WrapMode m_wrapMode = WM_CLAMP;
    bool m_mipmapped = true;

    shared_ptr<Image> m_image;
    size_t m_actualWidth = 0, m_actualHeight = 0;

    // GL data.
    unsigned int m_glid = 0;
    unsigned int m_glType;

    bool createGLTexture();

public:
    Texture();
    ~Texture();

    Filtering filtering() const { return m_filteringMode; }
    Type type() const { return m_textureType; }
    bool isMipmapped() const { return m_mipmapped; }
    WrapMode wrapMode() const { return m_wrapMode; }

    unsigned getGLId() const { return m_glid; }

    void setType(Type newType);
    void setFilteringMode(Filtering newFiltering);
    void setMipmapped(bool hasMipmaps);
    void setWrapMode(WrapMode mode);

    void setImage(shared_ptr<Image> image);
    shared_ptr<Image> getImage() const;

    size_t getOriginalWidth() const;
    size_t getOriginalHeight() const;
    size_t getActualWidth() const;
    size_t getActualHeight() const;

    bool bind(size_t unit);
    void unbind();
    // TODO:
    void recreate();
};

} }