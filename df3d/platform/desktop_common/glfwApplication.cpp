#include "glfwApplication.h"

#include "glfwKeyCodes.h"
#include <df3d/platform/AppDelegate.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/input/InputManager.h>
#include <df3d/engine/EngineCVars.h>
#include <df3d/lib/Utils.h>
#include <GLFW/glfw3.h>

namespace df3d {

extern bool EngineInit(EngineInitParams params);
extern void EngineShutdown();

namespace platform_impl {

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void textInputCallback(GLFWwindow *window, unsigned int codepoint);
static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
static void cursorPosCallback(GLFWwindow *window, double x, double y);
static void windowFocusCallback(GLFWwindow *window, int focus);

#define DF3D_EMULATE_TOUCHES

class DesktopAppState
{
    GLFWwindow *m_window = nullptr;
    AppDelegate *m_appDelegate;
    bool m_vsync = false;

public:
    DesktopAppState() = default;
    ~DesktopAppState() = default;

    bool init(AppDelegate *appDelegate)
    {
        DF3D_ASSERT(appDelegate != nullptr);
        m_appDelegate = appDelegate;

        // Create window and OpenGL context.
        if (!glfwInit())
            throw std::runtime_error("Failed to init glfw");

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        const auto params = m_appDelegate->getInitParams();

        GLFWmonitor *monitor = params.fullscreen ? glfwGetPrimaryMonitor() : nullptr;

        m_window = glfwCreateWindow(params.windowWidth, params.windowHeight, "libdf3d_window", monitor, nullptr);
        if (!m_window)
        {
            DFLOG_CRITICAL("Failed to init glfw window");
            return false;
        }

        // Center the window.
        auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if (videoMode && !params.fullscreen)
            glfwSetWindowPos(m_window, (videoMode->width - params.windowWidth) / 2, (videoMode->height - params.windowHeight) / 2);

        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, this);

        if (params.vsync)
            glfwSwapInterval(1);
        m_vsync = params.vsync;

        // Set input callbacks.
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetCharCallback(m_window, textInputCallback);
        glfwSetScrollCallback(m_window, scrollCallback);
        glfwSetCursorPosCallback(m_window, cursorPosCallback);
        glfwSetWindowFocusCallback(m_window, windowFocusCallback);

        // Init the engine.
        if (!EngineInit(params))
        {
            DFLOG_CRITICAL("Failed to init df3d");
            return false;
        }
        // Init game code.
        if (!m_appDelegate->onAppStarted())
        {
            DFLOG_CRITICAL("Game code initialization failed");
            return false;
        }

        return true;
    }

    void shutdown()
    {
        m_appDelegate->onAppEnded();

        EngineShutdown();

        if (m_window)
            glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void run()
    {
        using namespace std::chrono;

        while (!glfwWindowShouldClose(m_window))
        {
            const double prefFPS = EngineCVars::preferredFPS;
            const auto frameEnds = std::chrono::steady_clock::now() + std::chrono::duration<double>(1.0 / prefFPS);

            glfwPollEvents();
            svc().step();
            glfwSwapBuffers(m_window);

            /*
            if (m_vsync)
                std::this_thread::sleep_until(frameEnds);
                */
        }
    }

    void setTitle(const std::string &title)
    {
        glfwSetWindowTitle(m_window, title.c_str());
    }

    void onMouseButton(int button, int action, int mods)
    {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);

        MouseButton df3dBtn;

        if (button == GLFW_MOUSE_BUTTON_LEFT)
            df3dBtn = MouseButton::LEFT;
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            df3dBtn = MouseButton::RIGHT;
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
            df3dBtn = MouseButton::MIDDLE;
        else
            return;

        if (action == GLFW_PRESS)
            svc().inputManager().onMouseButtonPressed(df3dBtn, (int)x, (int)y);
        else if (action == GLFW_RELEASE)
            svc().inputManager().onMouseButtonReleased(df3dBtn, (int)x, (int)y);

#ifdef DF3D_EMULATE_TOUCHES
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
                svc().inputManager().onTouch(0, (int)x, (int)y, Touch::State::DOWN);
            else if (action == GLFW_RELEASE)
                svc().inputManager().onTouch(0, (int)x, (int)y, Touch::State::UP);
        }
#endif
    }

    void onKey(int key, int scancode, int action, int mods)
    {
        int keyModifiers = 0;
        if (mods & GLFW_MOD_SHIFT)
            keyModifiers |= KeyModifier::KM_SHIFT;
        if (mods & GLFW_MOD_ALT)
            keyModifiers |= KeyModifier::KM_ALT;
        if (mods & GLFW_MOD_CONTROL)
            keyModifiers |= KeyModifier::KM_CTRL;

        KeyCode df3dCode = convertGlfwKeyCode(key);
        KeyModifier df3dModifiers = static_cast<KeyModifier>(keyModifiers);

        if (df3dCode == KeyCode::UNDEFINED)
        {
            DFLOG_WARN("Undefined key input!");
            return;
        }

        if (action == GLFW_PRESS)
            svc().inputManager().onKeyDown(df3dCode, df3dModifiers);
        else if (action == GLFW_RELEASE)
            svc().inputManager().onKeyUp(df3dCode, df3dModifiers);
    }

    void onTextInput(unsigned int codepoint)
    {
        svc().inputManager().onTextInput(codepoint);
    }

    void onScroll(double xoffset, double yoffset)
    {
        svc().inputManager().setMouseWheelDelta((float)-yoffset);
    }

    void onCursorMove(double x, double y)
    {
        svc().inputManager().setMousePosition((int)x, (int)y);

#ifdef DF3D_EMULATE_TOUCHES
        int state = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_PRESS)
            svc().inputManager().onTouch(0, (int)x, (int)y, Touch::State::MOVING);
#endif
    }

    void onFocus(int focused)
    {
        if (focused == 1)
        {
            m_appDelegate->onAppWillEnterForeground();
            m_appDelegate->onAppDidBecomeActive();
        }
        else
        {
            m_appDelegate->onAppWillResignActive();
            m_appDelegate->onAppDidEnterBackground();
        }
    }
};

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    auto app = reinterpret_cast<DesktopAppState*>(glfwGetWindowUserPointer(window));

    app->onMouseButton(button, action, mods);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto app = reinterpret_cast<DesktopAppState*>(glfwGetWindowUserPointer(window));

    app->onKey(key, scancode, action, mods);
}

static void textInputCallback(GLFWwindow *window, unsigned int codepoint)
{
    auto app = reinterpret_cast<DesktopAppState*>(glfwGetWindowUserPointer(window));

    app->onTextInput(codepoint);
}

static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    auto app = reinterpret_cast<DesktopAppState*>(glfwGetWindowUserPointer(window));

    app->onScroll(xoffset, yoffset);
}

static void cursorPosCallback(GLFWwindow *window, double x, double y)
{
    auto app = reinterpret_cast<DesktopAppState*>(glfwGetWindowUserPointer(window));

    app->onCursorMove(x, y);
}

static void windowFocusCallback(GLFWwindow *window, int focus)
{
    auto app = reinterpret_cast<DesktopAppState*>(glfwGetWindowUserPointer(window));

    app->onFocus(focus);
}

static DesktopAppState g_application;

void glfwAppRun()
{
    MemoryManager::init();

    if (g_application.init(df3d_GetAppDelegate()))
        g_application.run();

    g_application.shutdown();

    MemoryManager::shutdown();
}

} }

namespace df3d {

void Application::setTitle(const std::string &title)
{
    platform_impl::g_application.setTitle(title);
}

}
