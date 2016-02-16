#pragma once

namespace df3d {

class DF3D_DLL FrameStats
{
public:
    size_t drawCalls = 0;
    size_t totalTriangles = 0;
    size_t totalLines = 0;

    size_t totalLights = 0;
};

}
