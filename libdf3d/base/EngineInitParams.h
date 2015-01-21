#pragma once

namespace df3d { namespace base { 

struct DF3D_DLL EngineInitParams
{
    // All params will be gotten from the config file if this field is set.
    const char *configFile = nullptr;

    int argc = 0;
    char **argv = nullptr;

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