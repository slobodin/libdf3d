#pragma once

namespace df3d { namespace render {

// TODO_REFACTO rename this file.

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

extern const DF3D_DLL int ANISOTROPY_LEVEL_MAX;
extern const DF3D_DLL int NO_ANISOTROPY;

// TODO_REFACTO - 

class DF3D_DLL RenderingCapabilities
{
public:
    TextureFiltering textureFiltering = TextureFiltering::TRILINEAR;
    bool mipmaps = true;
    int anisotropyMax = ANISOTROPY_LEVEL_MAX;

    static RenderingCapabilities getDefaults();
};

} }
