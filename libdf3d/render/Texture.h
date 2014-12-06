#pragma once

namespace df3d { namespace render {

extern const DF3D_DLL int ANISOTROPY_LEVEL_MAX;

class Image;

class Texture
{
public:
    enum class Filtering
    {
        NEAREST,
        BILINEAR,
        TRILINEAR
    };

    enum class Type
    {
        NONE,
        TEXTURE_1D,
        TEXTURE_2D,
        TEXTURE_3D,
        TEXTURE_CUBE
    };

    enum class WrapMode
    {
        WRAP,
        CLAMP
    };

private:
    Filtering m_filteringMode = Filtering::TRILINEAR;
    Type m_textureType = Type::TEXTURE_2D;
    WrapMode m_wrapMode = WrapMode::CLAMP;
    bool m_mipmapped = true;
    int m_maxAnisotropy = 1;

    shared_ptr<Image> m_image;
    bool m_imageDirty = true;
    size_t m_actualWidth = 0, m_actualHeight = 0;

    // GL data.
    unsigned int m_glid = 0;
    unsigned int m_glType;

    bool createGLTexture();
    void deleteGLTexture();

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
    void setMaxAnisotropy(int aniso);

    void setImage(shared_ptr<Image> image);
    shared_ptr<const Image> getImage() const;

    size_t getOriginalWidth() const;
    size_t getOriginalHeight() const;
    size_t getActualWidth() const;
    size_t getActualHeight() const;

    bool bind(size_t unit);
    void unbind();
};

} }