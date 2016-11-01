#pragma once

#include <SPARK.h>

namespace df3d {

class ParticleSystemUtils
{
public:
    static SPK::Ref<SPK::Renderer> createQuadRenderer(const glm::vec2 &scale, ResourceID textureResource, bool depthTest);
    static SPK::Ref<SPK::Renderer> createTrailRenderer(size_t nbSamples, float duration, float width, bool depthTest);
};

}
