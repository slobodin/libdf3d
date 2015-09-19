#include "RenderCommon.h"

namespace df3d { namespace render {

const int ANISOTROPY_LEVEL_MAX = -1;
const int NO_ANISOTROPY = 1;

RenderingCapabilities RenderingCapabilities::getDefaults()
{
    return RenderingCapabilities();
}

} }
