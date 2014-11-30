#pragma once

#include <base/InputEvents.h>

FWD_MODULE_CLASS(base, Controller)

namespace df3d { namespace platform {

class AppEvent
{
public:
    enum class Type
    {
        QUIT,
        KEYBOARD,
        MOUSE_BUTTON,
        MOUSE_MOTION,
        MOUSE_WHEEL,
        FINGER
    };

    Type type;

    base::MouseMotionEvent mouseMotion;
    base::MouseButtonEvent mouseButton;
    base::MouseWheelEvent mouseWheel;
    base::KeyboardEvent keyboard;
};

struct AppInitParams
{
    int windowWidth = DEFAULT_WINDOW_WIDTH;
    int windowHeight = DEFAULT_WINDOW_HEIGHT;
    bool fullscreen = false;
};

class Application
{
    friend class base::Controller;

protected:
    Application();
    virtual ~Application();

    virtual bool init(AppInitParams params) = 0;
    virtual void shutdown() = 0;

    virtual bool pollEvents() = 0;
    virtual void swapBuffers() = 0;

    virtual void setTitle(const char *title) = 0;
};

} }