#pragma once

#include "../Application.h"

namespace df3d { namespace platform {

#if defined(__ANDROID__)

class SDLAndroidApplication : public Application
{
    friend class base::Controller;

    SDL_Window *m_window;
    SDL_GLContext m_glContext;
protected:
    SDLAndroidApplication();
    virtual ~SDLAndroidApplication();

    bool init(AppInitParams params);
    void shutdown();

    void pollEvents();
    void swapBuffers();

    void setTitle(const char *title);
};

#endif

} }
