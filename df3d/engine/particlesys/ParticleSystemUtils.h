#pragma once

#include "ParticleSystemComponentProcessor.h"

namespace df3d {

class DF3D_DLL ParticleSystemUtils
{
public:
    static ParticleSystemCreationParams parseVfx(const char *vfxFile);
    static SPK::Ref<SPK::Renderer> createQuadRenderer(const glm::vec2 &scale, const std::string &texturePath, bool depthTest);
    static SPK::Ref<SPK::Renderer> createTrailRenderer(size_t nbSamples, float duration, float width, bool depthTest);
    static void enableFaceCulling(SPK::Ref<SPK::Renderer> renderer, bool enable);
};

}
