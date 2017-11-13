#pragma once

#include "RenderOperation.h"
#include "RenderCommon.h"
#include <df3d/engine/3d/Light.h>

namespace df3d {

struct RenderQueue
{
    std::vector<RenderOperation> rops[RQ_BUCKET_COUNT];
    Light lights[LIGHTS_MAX];

    void sort();
    void clear();
};

}
