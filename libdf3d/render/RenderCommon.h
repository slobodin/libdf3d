#pragma once

namespace df3d { namespace render {

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

enum CubeFace
{
    CUBE_FACE_POSITIVE_X,
    CUBE_FACE_NEGATIVE_X,
    CUBE_FACE_POSITIVE_Y,
    CUBE_FACE_NEGATIVE_Y,
    CUBE_FACE_POSITIVE_Z,
    CUBE_FACE_NEGATIVE_Z,

    CUBE_FACES_COUNT
};

extern const DF3D_DLL int ANISOTROPY_LEVEL_MAX;
extern const DF3D_DLL int NO_ANISOTROPY;

// FIXME: don't like it.
class DF3D_DLL RenderingCapabilities
{
public:
    TextureFiltering textureFiltering = TextureFiltering::TRILINEAR;
    bool mipmaps = true;
    int anisotropyMax = ANISOTROPY_LEVEL_MAX;

    static RenderingCapabilities getDefaults();
};

} }
