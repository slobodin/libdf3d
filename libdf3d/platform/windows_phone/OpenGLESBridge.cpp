#include "df3d_pch.h"
#include "OpenGLESBridge.h"

using namespace Windows::Foundation::Collections;
using namespace Platform;

namespace df3d { namespace platform {

inline float ConvertDipsToPixels(float dips, float dpi)
{
    static const float dipsPerInch = 96.0f;
    return floor(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
}

OpenGLESBridge::OpenGLESBridge()
{

}

OpenGLESBridge::~OpenGLESBridge()
{
    cleanupEGL();
}

void OpenGLESBridge::initEGL(Windows::UI::Core::CoreWindow ^window)
{
    const EGLint configAttributes[] =
    {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    const EGLint contextAttributes[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    const EGLint surfaceAttributes[] =
    {
        // EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER is part of the same optimization as EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER (see above).
        // If you have compilation issues with it then please update your Visual Studio templates.
        EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_NONE
    };

    const EGLint defaultDisplayAttributes[] =
    {
        // These are the default display attributes, used to request ANGLE's D3D11 renderer.
        // eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

        // EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
        // Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_NONE,
    };

    const EGLint fl9_3DisplayAttributes[] =
    {
        // These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
        // These attributes are used if the call to eglInitialize fails with the default display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_NONE,
    };

    const EGLint warpDisplayAttributes[] =
    {
        // These attributes can be used to request D3D11 WARP.
        // They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_USE_WARP_ANGLE, EGL_TRUE,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_NONE,
    };

    EGLConfig config = NULL;

    // eglGetPlatformDisplayEXT is an alternative to eglGetDisplay. It allows us to pass in display attributes, used to configure D3D11.
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
    if (!eglGetPlatformDisplayEXT)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to get function eglGetPlatformDisplayEXT");
    }

    //
    // To initialize the display, we make three sets of calls to eglGetPlatformDisplayEXT and eglInitialize, with varying 
    // parameters passed to eglGetPlatformDisplayEXT:
    // 1) The first calls uses "defaultDisplayAttributes" as a parameter. This corresponds to D3D11 Feature Level 10_0+.
    // 2) If eglInitialize fails for step 1 (e.g. because 10_0+ isn't supported by the default GPU), then we try again 
    //    using "fl9_3DisplayAttributes". This corresponds to D3D11 Feature Level 9_3.
    // 3) If eglInitialize fails for step 2 (e.g. because 9_3+ isn't supported by the default GPU), then we try again 
    //    using "warpDisplayAttributes".  This corresponds to D3D11 Feature Level 11_0 on WARP, a D3D11 software rasterizer.
    //
    // Note: On Windows Phone, we #ifdef out the first set of calls to eglPlatformDisplayEXT and eglInitialize.
    //       Windows Phones devices only support D3D11 Feature Level 9_3, but the Windows Phone emulator supports 11_0+.
    //       We use this #ifdef to limit the Phone emulator to Feature Level 9_3, making it behave more like
    //       real Windows Phone devices.
    //       If you wish to test Feature Level 10_0+ in the Windows Phone emulator then you should remove this #ifdef.
    //

#if (WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP)
    // This tries to initialize EGL to D3D11 Feature Level 10_0+. See above comment for details.
    m_EglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
    if (m_EglDisplay == EGL_NO_DISPLAY)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to get EGL display");
    }

    if (eglInitialize(m_EglDisplay, NULL, NULL) == EGL_FALSE)
#endif    
    {
        // This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on Windows Phone, or certain Windows tablets).
        m_EglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
        if (m_EglDisplay == EGL_NO_DISPLAY)
        {
            throw Exception::CreateException(E_FAIL, L"Failed to get EGL display");
        }

        if (eglInitialize(m_EglDisplay, NULL, NULL) == EGL_FALSE)
        {
            // This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU (e.g. on Surface RT).
            m_EglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
            if (m_EglDisplay == EGL_NO_DISPLAY)
            {
                throw Exception::CreateException(E_FAIL, L"Failed to get EGL display");
            }

            if (eglInitialize(m_EglDisplay, NULL, NULL) == EGL_FALSE)
            {
                // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
                throw Exception::CreateException(E_FAIL, L"Failed to initialize EGL");
            }
        }
    }

    EGLint numConfigs = 0;
    if ((eglChooseConfig(m_EglDisplay, configAttributes, &config, 1, &numConfigs) == EGL_FALSE) || (numConfigs == 0))
    {
        throw Exception::CreateException(E_FAIL, L"Failed to choose first EGLConfig");
    }

    // Create a PropertySet and initialize with the EGLNativeWindowType.
    PropertySet^ surfaceCreationProperties = ref new PropertySet();
    surfaceCreationProperties->Insert(ref new String(EGLNativeWindowTypeProperty), window);

    //
    // A Custom render surface size can be specified by uncommenting the following lines.
    // The render surface will be automatically scaled to fit the entire window.  Using a
    // smaller sized render surface can result in a performance gain.
    //
    //m_customRenderSurfaceSize = Size(800, 600);
    //m_useCustomRenderSurfaceSize = true;
    //surfaceCreationProperties->Insert(ref new String(EGLRenderSurfaceSizeProperty), PropertyValue::CreateSize(m_customRenderSurfaceSize));
    //

    m_EglSurface = eglCreateWindowSurface(m_EglDisplay, config, reinterpret_cast<IInspectable*>(surfaceCreationProperties), surfaceAttributes);
    if (m_EglSurface == EGL_NO_SURFACE)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to create EGL fullscreen surface");
    }

    m_EglContext = eglCreateContext(m_EglDisplay, config, EGL_NO_CONTEXT, contextAttributes);
    if (m_EglContext == EGL_NO_CONTEXT)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to create EGL context");
    }

    if (eglMakeCurrent(m_EglDisplay, m_EglSurface, m_EglSurface, m_EglContext) == EGL_FALSE)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to make fullscreen EGLSurface current");
    }
}

void OpenGLESBridge::cleanupEGL()
{
    if (m_EglDisplay != EGL_NO_DISPLAY && m_EglSurface != EGL_NO_SURFACE)
    {
        eglDestroySurface(m_EglDisplay, m_EglSurface);
        m_EglSurface = EGL_NO_SURFACE;
    }

    if (m_EglDisplay != EGL_NO_DISPLAY && m_EglContext != EGL_NO_CONTEXT)
    {
        eglDestroyContext(m_EglDisplay, m_EglContext);
        m_EglContext = EGL_NO_CONTEXT;
    }

    if (m_EglDisplay != EGL_NO_DISPLAY)
    {
        eglTerminate(m_EglDisplay);
        m_EglDisplay = EGL_NO_DISPLAY;
    }
}

void OpenGLESBridge::updateWindowSize(Windows::Foundation::Size size)
{
    Windows::Foundation::Size pixelSize;
    // Use the dimensions of the custom render surface size if one was specified.
    if (m_useCustomRenderSurfaceSize)
    {
        // Render surface size is already in pixels
        pixelSize = m_customRenderSurfaceSize;
    }
    else
    {
        Windows::Graphics::Display::DisplayInformation^ currentDisplayInformation = Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
        pixelSize = Windows::Foundation::Size(ConvertDipsToPixels(size.Width, currentDisplayInformation->LogicalDpi), ConvertDipsToPixels(size.Height, currentDisplayInformation->LogicalDpi));
    }

    m_windowWidth = static_cast<GLsizei>(pixelSize.Width);
    m_windowHeight = static_cast<GLsizei>(pixelSize.Height);
}

void OpenGLESBridge::swapBuffers()
{
    if (eglSwapBuffers(m_EglDisplay, m_EglSurface) != GL_TRUE)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to swap EGL buffers.");
        // TODO:
        // Should recreate EGL instead.
    }
}

} }