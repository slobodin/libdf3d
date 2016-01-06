#pragma once

#include "ParticleSystemComponentProcessor.h"

namespace df3d {

class DF3D_DLL ParticleSystemUtils
{
public:
    static ParticleSystemCreationParams parseVfx(const std::string &vfxFile);
    static SPK::Ref<SPK::Renderer> createQuadRenderer(const glm::vec2 &scale, const std::string &texturePath, bool depthTest);
};

}
