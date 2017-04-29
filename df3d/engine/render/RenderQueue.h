#pragma once

#include "RenderOperation.h"
#include "RenderCommon.h"
#include <df3d/engine/3d/Light.h>

namespace df3d {

struct RenderQueue
{
    std::vector<RenderOperation> litOpaqueOperations;
    std::vector<RenderOperation> notLitOpaqueOperations;
    std::vector<RenderOperation> transparentOperations;
    std::vector<RenderOperation2D> sprite2DOperations;
    std::vector<RenderOperation> debugDrawOperations;
    Light lights[LIGHTS_MAX];

    void sort();
    void clear();
};

}
