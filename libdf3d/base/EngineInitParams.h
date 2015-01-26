#pragma once

namespace df3d { namespace base { 

struct DF3D_DLL EngineInitParams
{
    int windowWidth = DEFAULT_WINDOW_WIDTH;
    int windowHeight = DEFAULT_WINDOW_HEIGHT;
    bool fullscreen = false;
    bool debugDraw = false;
    // TODO:
    // More params
    // More rendering params
    // Paths to the resources, etc
    // windowed, fullscreen mode
};

} }