#include "df3d_pch.h"
#include "RenderingCapabilities.h"

namespace df3d { namespace render {

const int ANISOTROPY_LEVEL_MAX = -1;

RenderingCapabilities RenderingCapabilities::getDefaults()
{
    RenderingCapabilities result;
    result.anisotropyMax = ANISOTROPY_LEVEL_MAX;
    result.mipmaps = true;
    result.textureFiltering = TextureFiltering::TRILINEAR;

    return result;
}

} }
