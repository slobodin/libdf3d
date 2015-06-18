#include "df3d_pch.h"
#include "LinuxApplication.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>
#include <GL/glew.h>
#include <GL/glx.h>

namespace df3d { namespace platform {

class AppWindow
{
    Display *m_display = nullptr;
    Window m_window = 0;
    Colormap m_cmap = 0;
    GLXContext m_context = nullptr;

    AppDelegate *m_appDelegate;

public:
    AppWindow(const AppInitParams &params, AppDelegate *appDelegate);
    ~AppWindow();

    void mainLoop();
    void setTitle(const char *title);
};

AppWindow::AppWindow(const AppInitParams &params, AppDelegate *appDelegate)
    : m_appDelegate(appDelegate)
{
    m_display = XOpenDisplay(nullptr);

    int glxMajor, glxMinor;

    if (!glXQueryVersion(m_display, &glxMajor, &glxMinor))
    {
        XCloseDisplay(m_display);
        throw std::runtime_error("Can't glXQueryVersion");
    }

    int fbcount;
    int visualAttribs[] =
    {
        GLX_X_RENDERABLE    , True,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        GLX_RED_SIZE        , 8,
        GLX_GREEN_SIZE      , 8,
        GLX_BLUE_SIZE       , 8,
        GLX_ALPHA_SIZE      , 8,
        GLX_DEPTH_SIZE      , 24,
        GLX_STENCIL_SIZE    , 8,
        GLX_DOUBLEBUFFER    , True,
        //GLX_SAMPLE_BUFFERS  , 1,
        //GLX_SAMPLES         , 4,
        None
    };

    GLXFBConfig *fbc = glXChooseFBConfig(m_display, DefaultScreen(m_display), visualAttribs, &fbcount);
    if (!fbc)
    {
        XCloseDisplay(m_display);
        throw std::runtime_error("Can't get framebuffer config.");
    }

    int bestFbc = -1, worstFbc = -1, bestNumSamp = -1, worstNumSamp = 999;
    for (int i = 0; i < fbcount; i++)
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig(m_display, fbc[i]);
        if (vi)
        {
            int sampBuf, samples;
            glXGetFBConfigAttrib(m_display, fbc[i], GLX_SAMPLE_BUFFERS, &sampBuf);
            glXGetFBConfigAttrib(m_display, fbc[i], GLX_SAMPLES, &samples);

            if (bestFbc < 0 || sampBuf && samples > bestNumSamp)
                bestFbc = i, bestNumSamp = samples;
            if (worstFbc < 0 || !sampBuf || samples < worstNumSamp)
                worstFbc = i, worstNumSamp = samples;
        }

        XFree(vi);
    }

    GLXFBConfig choosenFbc = fbc[bestFbc];
    XFree(fbc);

    XVisualInfo *vi = glXGetVisualFromFBConfig(m_display, choosenFbc);

    XSetWindowAttributes winAttrib;
    winAttrib.colormap = m_cmap = XCreateColormap(m_display, RootWindow(m_display, vi->screen), vi->visual, AllocNone);
    winAttrib.background_pixmap = None;
    winAttrib.border_pixel = 0;
    winAttrib.event_mask = StructureNotifyMask |
            KeyPressMask | KeyReleaseMask |
            ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ButtonMotionMask;

    m_window = XCreateWindow(m_display, RootWindow(m_display, vi->screen),
                             0, 0, params.windowWidth, params.windowHeight, 0, vi->depth, InputOutput,
                             vi->visual,
                             CWBorderPixel | CWColormap | CWEventMask, &winAttrib);
    if (!m_window)
    {
        XFree(vi);
        XCloseDisplay(m_display);
        XFreeColormap(m_display, m_cmap);

        throw std::runtime_error("Failed to create XWindow");
    }

    XStoreName(m_display, m_window, "libdf3d_window");
    XMapWindow(m_display, m_window);

    XFlush(m_display);
    XSync(m_display, False);

    // Create context.
    m_context = glXCreateContext(m_display, vi, NULL, GL_TRUE);
    XSync(m_display, False);

    XFree(vi);

    glXMakeCurrent(m_display, m_window, m_context);

    base::glog << "GL context created" << base::logmess;
}

AppWindow::~AppWindow()
{
    if (m_display)
    {
        glXMakeCurrent(m_display, 0, 0);
        glXDestroyContext(m_display, m_context);

        XDestroyWindow(m_display, m_window);
        XFreeColormap(m_display, m_cmap);
        XCloseDisplay(m_display);
    }
}

void AppWindow::mainLoop()
{
    using namespace std::chrono;

    TimePoint currtime, prevtime;
    currtime = prevtime = system_clock::now();

    Atom wmDeleteMessage = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_display, m_window, &wmDeleteMessage, 1);

    XEvent event;

    while (true)
    {
        while (XPending(m_display))
        {
            XNextEvent(m_display, &event);
            switch (event.type)
            {
            case ButtonPress:
            case ButtonRelease:
            {
                base::MouseButtonEvent ev;
                ev.x = event.xkey.x;
                ev.y = event.xkey.y;

                if (event.xbutton.button == 1)
                    ev.button = base::MouseButtonEvent::Button::LEFT;
                else if (event.xbutton.button == 2)
                    ev.button = base::MouseButtonEvent::Button::MIDDLE;
                else if (event.xbutton.button == 3)
                    ev.button = base::MouseButtonEvent::Button::RIGHT;

                if (event.type == ButtonPress)
                    ev.state = base::MouseButtonEvent::State::PRESSED;
                else
                    ev.state = base::MouseButtonEvent::State::RELEASED;

                m_appDelegate->onMouseButtonEvent(ev);

                // TODO:
                // Wheel.
            }
                break;

            case MotionNotify:
            {
                base::MouseMotionEvent ev;
                ev.x = event.xkey.x;
                ev.y = event.xkey.y;
                ev.leftPressed = event.xmotion.state & Button1MotionMask;
                ev.rightPressed = event.xmotion.state & Button3MotionMask;

                m_appDelegate->onMouseMotionEvent(ev);
            }
                break;

            case KeyPress:
            case KeyRelease:
            {

            }
                break;

            case ClientMessage:
                if (event.xclient.data.l[0] == wmDeleteMessage)
                    return;
            }
        }

        currtime = system_clock::now();

        m_appDelegate->onAppUpdate(IntervalBetween(currtime, prevtime));

        glXSwapBuffers(m_display, m_window);

        prevtime = currtime;
    }
}

void AppWindow::setTitle(const char *title)
{
    XStoreName(m_display, m_window, title);
}

struct LinuxApplication::Impl
{
    unique_ptr<AppWindow> window;
};

LinuxApplication::LinuxApplication(const AppInitParams &params, AppDelegate *appDelegate)
    : Application(params, appDelegate),
    m_pImpl(make_unique<Impl>())
{
    m_pImpl->window = make_unique<AppWindow>(params, appDelegate);

    // Init GLEW.
    glewExperimental = GL_TRUE;

    auto glewerr = glewInit();
    if (glewerr != GLEW_OK)
    {
        std::string errStr = "GLEW initialization failed: ";
        errStr += (const char *)glewGetErrorString(glewerr);
        throw std::runtime_error(errStr);
    }

    if (!glewIsSupported("GL_VERSION_2_1"))
        throw std::runtime_error("GL 2.1 unsupported");

    if (!m_appDelegate->onAppStarted(params.windowWidth, params.windowHeight))
        throw std::runtime_error("Game code initialization failed.");
}

LinuxApplication::~LinuxApplication()
{

}

void LinuxApplication::run()
{
    m_pImpl->window->mainLoop();

    m_appDelegate->onAppEnded();
    m_pImpl.reset();
}

void LinuxApplication::setTitle(const char *title)
{
    m_pImpl->window->setTitle(title);
}

} }
