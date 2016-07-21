#pragma once

#include <df3d/lib/Handles.h>

namespace df3d {

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

enum class Topology
{
    LINES,
    TRIANGLES,
    LINE_STRIP,
    TRIANGLE_STRIP
};

// TODO: refactor blending mode.
enum class BlendingMode : int
{
    NONE,
    ADDALPHA,
    ALPHA,
    ADD
};

enum class FaceCullMode : int
{
    NONE,
    FRONT,
    BACK
};

enum IndicesType
{
    INDICES_16_BIT,
    INDICES_32_BIT
};

DF3D_MAKE_SHORT_HANDLE(VertexBufferHandle)
DF3D_MAKE_SHORT_HANDLE(IndexBufferHandle)
DF3D_MAKE_SHORT_HANDLE(TextureHandle)
DF3D_MAKE_SHORT_HANDLE(ShaderHandle)
DF3D_MAKE_SHORT_HANDLE(GpuProgramHandle)
DF3D_MAKE_SHORT_HANDLE(UniformHandle)

struct DF3D_DLL FrameStats
{
    size_t drawCalls = 0;
    size_t totalTriangles = 0;
    size_t totalLines = 0;

    size_t textures = 0;
    size_t gpuMemBytes = 0;
};

// FIXME: don't like it.
class DF3D_DLL RenderingCapabilities
{
    TextureFiltering m_textureFiltering = TextureFiltering::TRILINEAR;
    bool m_mipmaps = true;

public:
    void setFiltering(TextureFiltering f) { m_textureFiltering = f; }
    void setHasMipmaps(bool mipmaps) { m_mipmaps = mipmaps; }

    TextureFiltering getFiltering() const { return m_textureFiltering; }
    bool hasMipmaps() const { return m_mipmaps; }

    static RenderingCapabilities getDefaults();
};

}
