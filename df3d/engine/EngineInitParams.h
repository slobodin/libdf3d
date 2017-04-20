#pragma once

#include <string>
#include <df3d/Common.h>

namespace df3d {

struct EngineInitParams
{
    int windowWidth = DEFAULT_WINDOW_WIDTH;
    int windowHeight = DEFAULT_WINDOW_HEIGHT;
    bool fullscreen = false;
    bool vsync = false;

    bool createConsole = false;
    // TODO:
    // More params
    // More rendering params
    // Paths to the resources, etc
    // windowed, fullscreen mode

    std::string physicsConfigPath;
};

}