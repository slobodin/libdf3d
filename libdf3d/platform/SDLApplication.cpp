#include "df3d_pch.h"
#include "SDLApplication.h"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <base/Controller.h>

namespace df3d { namespace platform {

SDLApplication::SDLApplication()
    : m_window(0), m_glContext(0)
{
}

SDLApplication::~SDLApplication()
{
}

bool SDLApplication::init(AppInitParams params)
{
#if defined(__WIN32__)
    // NOTE: SDL initialized at start point on Android.
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        base::glog << "Could not initialize SDL" << base::logcritical;
        return false;
    }
#endif

#if defined(__ANDROID__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#if defined(__WIN32__)
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#endif

    // FIXME:
    // This is renderer.init()
    unsigned int sdlInitFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (params.fullscreen)
        sdlInitFlags |= SDL_WINDOW_FULLSCREEN;

    int posx = 0, posy = 0;
#if defined(__WIN32__)
    posx = SDL_WINDOWPOS_CENTERED;
    posy = SDL_WINDOWPOS_CENTERED;
#endif

    m_window = SDL_CreateWindow("df3d application", posx, posy, params.windowWidth, params.windowHeight, sdlInitFlags);
    if (!m_window)
    {
        base::glog << "Can not set video mode" << base::logcritical;
        return false;
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext)
    {
        base::glog << "Can not create OpenGL context:" << SDL_GetError() << base::logcritical;
        return false;
    }

    // SDL_Image init.
    int imgLibInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    auto imgLibInitialized = IMG_Init(imgLibInitFlags);
    if ((imgLibInitFlags & imgLibInitialized) != imgLibInitFlags)
    {
        base::glog << "Couldn't load image support library due to" << IMG_GetError() << base::logcritical;
        return false;
    }

    // SDL_ttf init.
    if (TTF_Init() == -1)
    {
        base::glog << "Failed to initialize SDL ttf library" << TTF_GetError() << base::logcritical;
        return false;
    }

    return true;
}

void SDLApplication::shutdown()
{
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void SDLApplication::pollEvents()
{
    static SDL_Event sdlevent;

    while (SDL_PollEvent(&sdlevent))
    {
        g_engineController->dispatchAppEvent(&sdlevent);
    }
}

void SDLApplication::swapBuffers()
{
    // FIXME:
    // Create class RenderWindow with method swapBuffers
    SDL_GL_SwapWindow(m_window);
}

void SDLApplication::setTitle(const char *title)
{
#if defined(__WIN32__)
    SDL_SetWindowTitle(m_window, title);
#endif
}

} }