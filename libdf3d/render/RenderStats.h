#pragma once

namespace df3d { namespace render {

class DF3D_DLL RenderStats
{
public:
    size_t drawCalls = 0;
    size_t totalTriangles = 0;
    size_t totalNodes = 0;
    size_t totalParticleSystems = 0;
    size_t totalLights = 0;
    size_t totalParticles = 0;

    void reset()
    {
        drawCalls = totalTriangles = totalNodes = totalParticleSystems = totalLights = totalParticles = 0;
    }
};

} }