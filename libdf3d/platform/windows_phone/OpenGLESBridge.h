#pragma once

#include <angle_gl.h>
#include <angle_windowsstore.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace df3d { namespace platform {

class OpenGLESBridge
{
    EGLDisplay m_EglDisplay = EGL_NO_DISPLAY;
    EGLContext m_EglContext = EGL_NO_CONTEXT;
    EGLSurface m_EglSurface = EGL_NO_SURFACE;

    GLsizei m_windowWidth = 0, m_windowHeight = 0;

    Windows::Foundation::Size m_customRenderSurfaceSize;
    bool m_useCustomRenderSurfaceSize = false;

public:
    OpenGLESBridge();
    ~OpenGLESBridge();

    void initEGL(Windows::UI::Core::CoreWindow ^window);
    void cleanupEGL();

    void updateWindowSize(Windows::Foundation::Size size);
    void swapBuffers();
};

} }