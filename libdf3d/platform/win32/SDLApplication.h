#pragma once

#include "../Application.h"

#if defined(__WIN32__)

namespace df3d { namespace platform {

class SDLApplication : public Application
{
    friend class base::Controller;

    SDL_Window *m_window;
    SDL_GLContext m_glContext;

protected:
    SDLApplication();
    virtual ~SDLApplication();

    bool init(AppInitParams params);
    void shutdown();

    void pollEvents();
    void swapBuffers();

    void setTitle(const char *title);
};

} }

#endif // __WIN32__