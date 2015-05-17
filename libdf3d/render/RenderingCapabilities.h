#pragma once

namespace df3d { namespace render {

enum class TextureFiltering
{
    NEAREST,
    BILINEAR,
    TRILINEAR       // FIXME: uses LINEAR if mipmaps is off.
};

extern const DF3D_DLL int ANISOTROPY_LEVEL_MAX;
extern const DF3D_DLL int NO_ANISOTROPY;

class DF3D_DLL RenderingCapabilities
{
public:
    TextureFiltering textureFiltering = TextureFiltering::BILINEAR;
    bool mipmaps = false;
    int anisotropyMax = NO_ANISOTROPY;

    static RenderingCapabilities getDefaults();
};

} }
