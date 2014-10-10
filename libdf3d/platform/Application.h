#pragma once

FWD_MODULE_CLASS(base, Controller)

namespace df3d { namespace platform {

struct AppEvent
{
    virtual ~AppEvent() { }
};

struct AppInitParams
{
	int windowWidth = DEFAULT_WINDOW_WIDTH;
	int windowHeight = DEFAULT_WINDOW_HEIGHT;
	bool fullscreen = false;
};

class Application
{
protected:
    friend class base::Controller;

    Application();
    virtual ~Application();

    virtual bool init(AppInitParams params) = 0;
    virtual void shutdown() = 0;

    virtual void pollEvents() = 0;
    virtual void swapBuffers() = 0;

    virtual void setTitle(const char *title) = 0;
};

} }