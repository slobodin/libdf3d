#pragma once

#include "Application.h"

namespace df3d { namespace platform {

class SDLApplication : public Application
{
    friend class base::Controller;

    SDL_Window *m_window;
    SDL_GLContext m_glContext;

protected:
    SDLApplication();
    virtual ~SDLApplication();

    bool init(AppInitParams params) override;
    void shutdown() override;

    void pollEvents() override;
    void swapBuffers() override;

    void setTitle(const char *title) override;
};

} }