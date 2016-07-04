#pragma once

#include "RenderOperation.h"

namespace df3d {

class Light;

class RenderQueue
{
public:
    // FIXME:
    // Make not public.

    // FIXME: xxx!!!
    std::vector<RenderOperation> skybox;

    std::vector<RenderOperation> litOpaqueOperations;
    std::vector<RenderOperation> notLitOpaqueOperations;
    std::vector<RenderOperation> transparentOperations;
    std::vector<RenderOperation2D> sprite2DOperations;
    std::vector<RenderOperation> debugDrawOperations;
    std::vector<const Light*> lights;

    void sort();
    void clear();
};

}
