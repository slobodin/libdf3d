#pragma once

#include "RenderOperation.h"

FWD_MODULE_CLASS(components, LightComponent)

namespace df3d { namespace render {

class RenderQueue
{
public:
    // FIXME:
    // Make not public.

    std::vector<RenderOperation> litOpaqueOperations;
    std::vector<RenderOperation> notLitOpaqueOperations;
    std::vector<RenderOperation> transparentOperations;
    std::vector<RenderOperation> debugDrawOperations;
    std::vector<components::LightComponent *> lights;

    void sort();
    void clear();
};

} }