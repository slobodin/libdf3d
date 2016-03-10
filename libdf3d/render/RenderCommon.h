#pragma once

namespace df3d {

// FIXME:
// Make setting.
#define INDICES_16_BIT uint16_t
#define INDICES_32_BIT uint32_t

using INDICES_TYPE = INDICES_32_BIT;
using IndexArray = std::vector<INDICES_TYPE>;

enum class TextureFiltering
{
    NEAREST,
    BILINEAR,
    TRILINEAR       // FIXME: uses LINEAR if mipmaps is off.
};

enum class TextureWrapMode
{
    WRAP,
    CLAMP
};

//! Hint to graphics backend as to how a buffer's data will be accessed.
enum class GpuBufferUsageType
{
    STATIC,     /*!< The data store contents will be modified once and used many times. */
    DYNAMIC,    /*!< The data store contents will be modified repeatedly and used many times. */
    STREAM      /*!< The data store contents will be modified once and used at most a few times. */
};

enum class CubeFace
{
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z,

    COUNT
};

enum class ShaderType
{
    VERTEX,
    FRAGMENT,

    UNDEFINED
};

enum class RopType
{
    LINES,
    TRIANGLES,
    LINE_STRIP
};

// TODO: refactor blending mode.
enum class BlendingMode
{
    NONE,
    ADDALPHA,
    ALPHA,
    ADD
};

enum class FaceCullMode
{
    NONE,
    FRONT,
    BACK,
    FRONT_AND_BACK
};

#define DF3D_MAKE_DESCRIPTOR(name) struct name { int16_t id; name(int16_t id = -1) : id(id) { } bool valid() const { return id != -1; } }

DF3D_MAKE_DESCRIPTOR(VertexBufferDescriptor);
DF3D_MAKE_DESCRIPTOR(IndexBufferDescriptor);
DF3D_MAKE_DESCRIPTOR(TextureDescriptor);
DF3D_MAKE_DESCRIPTOR(ShaderDescriptor);
DF3D_MAKE_DESCRIPTOR(GpuProgramDescriptor);
DF3D_MAKE_DESCRIPTOR(UniformDescriptor);

// TODO_render
// FIXME: move to namespace.

extern const DF3D_DLL int ANISOTROPY_LEVEL_MAX;
extern const DF3D_DLL int NO_ANISOTROPY;

// FIXME: don't like it.
class DF3D_DLL RenderingCapabilities
{
    TextureFiltering m_textureFiltering = TextureFiltering::TRILINEAR;
    bool m_mipmaps = true;
    int m_anisotropyMax = ANISOTROPY_LEVEL_MAX;

public:
    void setFiltering(TextureFiltering f) { m_textureFiltering = f; }
    void setHasMipmaps(bool mipmaps) { m_mipmaps = mipmaps; }
    void setAnisotropy(int lvl) { m_anisotropyMax = lvl; }

    TextureFiltering getFiltering() const { return m_textureFiltering; }
    bool hasMipmaps() const { return m_mipmaps; }
    int getAnisotropy() const { return m_anisotropyMax; }

    static RenderingCapabilities getDefaults();
};

}
