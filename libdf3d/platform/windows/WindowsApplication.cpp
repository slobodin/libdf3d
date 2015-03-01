#include "df3d_pch.h"
#include "WindowsApplication.h"

#include <base/SystemsMacro.h>
#include "WindowsKeyCodes.h"

#define GLEW_STATIC
#include <GL/glew.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace df3d { namespace platform {

const char *WINDOW_CLASS_NAME = "libdf3d_window_class";
const char *WINDOW_NAME = "libdf3d_window";

class AppWindow
{
    HWND m_hWnd = nullptr;
    HDC m_hDC = nullptr;
    HGLRC m_hglRC = nullptr;
    bool m_isActive = false;

    bool m_quitRequested = false;

    AppDelegate *m_appDelegate = nullptr;

    DWORD getStyle(const AppInitParams &params);
    DWORD getExStyle(const AppInitParams &params);

public:
    AppWindow(const AppInitParams &params, AppDelegate *appDelegate);
    ~AppWindow();

    LRESULT handleMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    void swapBuffers();
    void setTitle(const char *title);

    bool quitRequested() const { return m_quitRequested; }
};

struct WindowSetupParam
{
    AppWindow *window = nullptr;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_CREATE)
    {
        LPCREATESTRUCT cs = (LPCREATESTRUCT)lp;
        WindowSetupParam *param = (WindowSetupParam*)cs->lpCreateParams;
        SetWindowLong(hwnd, GWL_USERDATA, (long)param->window);
    }
    else
    {
        AppWindow* win = (AppWindow*)GetWindowLong(hwnd, GWL_USERDATA);
        if (win)
            return win->handleMessage(hwnd, msg, wp, lp);
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

DWORD AppWindow::getStyle(const AppInitParams &params)
{
    if (params.fullscreen)
        return WS_POPUP;
    else
        return  WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU;
}

DWORD AppWindow::getExStyle(const AppInitParams &params)
{
    if (params.fullscreen)
        return WS_EX_APPWINDOW;
    else
        return WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
}

AppWindow::AppWindow(const AppInitParams &params, AppDelegate *appDelegate)
    : m_appDelegate(appDelegate)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEXA);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = WINDOW_CLASS_NAME;
    wcex.hIconSm = 0;

    if (!RegisterClassEx(&wcex))
        throw std::runtime_error("Failed to register window class.");

    WindowSetupParam param;
    param.window = this;

    // Get actual size.
    RECT rect;
    rect.left = 0;
    rect.right = params.windowWidth;
    rect.top = 0;
    rect.bottom = params.windowHeight;

    auto style = getStyle(params);
    auto styleEx = getExStyle(params);
    AdjustWindowRectEx(&rect, style, FALSE, styleEx);

    m_hWnd = CreateWindowEx(styleEx, WINDOW_CLASS_NAME, WINDOW_NAME, style, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, hInstance, &param);
    if (!m_hWnd)
    {
        UnregisterClass(WINDOW_CLASS_NAME, hInstance);
        throw std::runtime_error("Failed to create a window.");
    }

    m_hDC = GetDC(m_hWnd);
    if (!m_hDC)
    {
        UnregisterClass(WINDOW_CLASS_NAME, hInstance);
        DestroyWindow(m_hWnd);
        throw std::runtime_error("Failed to get device context");
    }

    // Init OpenGL.
    // FIXME: What to do with DirectX?

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat;
    pixelFormat = ChoosePixelFormat(m_hDC, &pfd);

    if (!pixelFormat)
        throw std::runtime_error("Failed to find pixel format.");

    if (!SetPixelFormat(m_hDC, pixelFormat, &pfd))
        throw std::runtime_error("Failed to set pixel format.");

    m_hglRC = wglCreateContext(m_hDC);
    if (!m_hglRC)
        throw std::runtime_error("Failed to create rendering context.");

    if (!wglMakeCurrent(m_hDC, m_hglRC))
        throw std::runtime_error("Failed to make context in this window.");

    // Center the window.
    GetWindowRect(m_hWnd, &rect);
    int screenX = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
    int screenY = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
    SetWindowPos(m_hWnd, m_hWnd, screenX, screenY, -1, -1, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);

    ShowWindow(m_hWnd, SW_SHOW);
}

void AppWindow::swapBuffers()
{
    SwapBuffers(m_hDC);
}

AppWindow::~AppWindow()
{
    wglMakeCurrent(NULL, NULL);

    if (m_hglRC)
        wglDeleteContext(m_hglRC);

    if (m_hDC)
        ReleaseDC(m_hWnd, m_hDC);
    if (m_hWnd)
        DestroyWindow(m_hWnd);
}

LRESULT AppWindow::handleMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static AppEvent ev;

    switch (msg)
    {
    case WM_KEYDOWN:
        ev.type = AppEvent::Type::KEYBOARD;
        ev.keyboard.keycode = convertKeyCode(wp);
        ev.keyboard.state = base::KeyboardEvent::State::PRESSED;
        g_engineController->dispatchAppEvent(ev);
        m_appDelegate->onKeyDown(ev.keyboard.keycode);
        break;

    case WM_KEYUP:
        ev.type = AppEvent::Type::KEYBOARD;
        ev.keyboard.keycode = convertKeyCode(wp);
        ev.keyboard.state = base::KeyboardEvent::State::RELEASED;
        g_engineController->dispatchAppEvent(ev);
        m_appDelegate->onKeyUp(ev.keyboard.keycode);
        break;

    case WM_MOUSEMOVE:
        ev.type = AppEvent::Type::MOUSE_MOTION;
        ev.mouseMotion.x = (short)LOWORD(lp);
        ev.mouseMotion.y = (short)HIWORD(lp);
        ev.mouseMotion.leftPressed = (wp & MK_LBUTTON) == MK_LBUTTON;
        ev.mouseMotion.rightPressed = (wp & MK_RBUTTON) == MK_RBUTTON;
        g_engineController->dispatchAppEvent(ev);
        m_appDelegate->onMouseMotionEvent(ev.mouseMotion);
        break;

    case WM_MOUSEWHEEL:
        ev.type = AppEvent::Type::MOUSE_WHEEL;
        g_engineController->dispatchAppEvent(ev);
        break;

    case WM_LBUTTONDOWN:
        ev.type = AppEvent::Type::MOUSE_BUTTON;
        ev.mouseButton.x = (short)LOWORD(lp);
        ev.mouseButton.y = (short)HIWORD(lp);
        ev.mouseButton.state = base::MouseButtonEvent::State::PRESSED;
        ev.mouseButton.button = base::MouseButtonEvent::Button::LEFT;
        g_engineController->dispatchAppEvent(ev);
        m_appDelegate->onMouseButtonEvent(ev.mouseButton);
        break;

    case WM_LBUTTONUP:
        ev.type = AppEvent::Type::MOUSE_BUTTON;
        ev.mouseButton.x = (short)LOWORD(lp);
        ev.mouseButton.y = (short)HIWORD(lp);
        ev.mouseButton.state = base::MouseButtonEvent::State::RELEASED;
        ev.mouseButton.button = base::MouseButtonEvent::Button::LEFT;
        g_engineController->dispatchAppEvent(ev);
        m_appDelegate->onMouseButtonEvent(ev.mouseButton);
        break;

    case WM_RBUTTONDOWN:
        ev.type = AppEvent::Type::MOUSE_BUTTON;
        ev.mouseButton.x = (short)LOWORD(lp);
        ev.mouseButton.y = (short)HIWORD(lp);
        ev.mouseButton.state = base::MouseButtonEvent::State::PRESSED;
        ev.mouseButton.button = base::MouseButtonEvent::Button::RIGHT;
        g_engineController->dispatchAppEvent(ev);
        m_appDelegate->onMouseButtonEvent(ev.mouseButton);
        break;

    case WM_RBUTTONUP:
        ev.type = AppEvent::Type::MOUSE_BUTTON;
        ev.mouseButton.x = (short)LOWORD(lp);
        ev.mouseButton.y = (short)HIWORD(lp);
        ev.mouseButton.state = base::MouseButtonEvent::State::RELEASED;
        ev.mouseButton.button = base::MouseButtonEvent::Button::RIGHT;
        g_engineController->dispatchAppEvent(ev);
        m_appDelegate->onMouseButtonEvent(ev.mouseButton);
        break;

    case WM_MBUTTONDOWN:
        ev.type = AppEvent::Type::MOUSE_BUTTON;
        ev.mouseButton.x = (short)LOWORD(lp);
        ev.mouseButton.y = (short)HIWORD(lp);
        ev.mouseButton.state = base::MouseButtonEvent::State::PRESSED;
        ev.mouseButton.button = base::MouseButtonEvent::Button::MIDDLE;
        g_engineController->dispatchAppEvent(ev);
        m_appDelegate->onMouseButtonEvent(ev.mouseButton);
        break;

    case WM_MBUTTONUP:
        ev.type = AppEvent::Type::MOUSE_BUTTON;
        ev.mouseButton.x = (short)LOWORD(lp);
        ev.mouseButton.y = (short)HIWORD(lp);
        ev.mouseButton.state = base::MouseButtonEvent::State::RELEASED;
        ev.mouseButton.button = base::MouseButtonEvent::Button::MIDDLE;
        g_engineController->dispatchAppEvent(ev);
        m_appDelegate->onMouseButtonEvent(ev.mouseButton);
        break;

    case WM_ACTIVATE:
        m_isActive = (LOWORD(wp) != WA_INACTIVE);
        break;

    case WM_DESTROY:
        m_quitRequested = true;
        return 0;

    case WM_SIZE:
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

void AppWindow::setTitle(const char *title)
{
    if (m_hWnd)
        SetWindowText(m_hWnd, title);
}

struct WindowsApplication::Impl
{
    unique_ptr<AppWindow> window;
};

WindowsApplication::WindowsApplication(const AppInitParams &params, AppDelegate *appDelegate)
    : Application(params, appDelegate),
    m_pImpl(make_unique<Impl>())
{
    m_pImpl->window = make_unique<AppWindow>(m_appInitParams, appDelegate);

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

WindowsApplication::~WindowsApplication()
{

}

void WindowsApplication::run()
{
    using namespace std::chrono;
    MSG msg;

    TimePoint currtime, prevtime;
    currtime = prevtime = system_clock::now();

    while (!m_pImpl->window->quitRequested())
    {
        // FIXME:
        // Maybe queue messages?
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            currtime = system_clock::now();

            m_appDelegate->onAppUpdate(IntervalBetween(currtime, prevtime));

            m_pImpl->window->swapBuffers();

            prevtime = currtime;
        }
    }

    m_appDelegate->onAppEnded();

    m_pImpl.reset();
}

void WindowsApplication::setTitle(const char *title)
{
    m_pImpl->window->setTitle(title);
}

} }
