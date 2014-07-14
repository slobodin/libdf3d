#include "df3d_pch.h"
#include "AndroidSDLApplication.h"

#include <SDL_image.h>
#include <base/Controller.h>

#if defined(__ANDROID__)

namespace df3d { namespace platform {

SDLAndroidApplication::SDLAndroidApplication()
	: m_window(0), m_glContext(0)
{
}

SDLAndroidApplication::~SDLAndroidApplication()
{
}

bool SDLAndroidApplication::init(AppInitParams params)
{
    // NOTE: SDL initialized at start point.
    /*if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        base::glog << "Could not initialize SDL" << base::logcritical;
        return false;
    }*/

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    unsigned int sdlInitFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (params.fullscreen)
        sdlInitFlags |= SDL_WINDOW_FULLSCREEN;
        
    // FIXME:
    // Use SDL_WINDOW_FULLSCREEN_DESKTOP

    m_window = SDL_CreateWindow(nullptr, 0, 0,
    		params.windowWidth, params.windowHeight, sdlInitFlags);
    if (!m_window)
    {
    	base::glog << "Can not set video mode" << base::logcritical;
    	return false;
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext)
    {
    	base::glog << "Can not create SDL GL context due to" << SDL_GetError() << base::logcritical;
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

    base::glog << "SDL successfully initialized" << base::logmess;

    return true;
}

void SDLAndroidApplication::shutdown()
{
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    IMG_Quit();
    SDL_Quit();
}

void SDLAndroidApplication::pollEvents()
{
    static SDL_Event sdlevent;

    while (SDL_PollEvent(&sdlevent))
    {
        g_engineController->dispatchAppEvent(&sdlevent);
    }
}

void SDLAndroidApplication::swapBuffers()
{
    // FIXME:
    // Create class RenderWindow with method swapBuffers
    SDL_GL_SwapWindow(m_window);
}

void SDLAndroidApplication::setTitle(const char *title)
{
    // SDL_SetWindowTitle(m_window, title.c_str());
}

} }

#endif // __ANDROID__
