#include "glfwApplication.h"

#include "glfwKeyCodes.h"
#include <libdf3d/platform/AppDelegate.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/input/InputManager.h>
#include <GLFW/glfw3.h>

namespace df3d { namespace platform_impl {

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void textInputCallback(GLFWwindow *window, unsigned int codepoint);
static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
static void cursorPosCallback(GLFWwindow *window, double x, double y);

#define DF3D_EMULATE_TOUCHES

class glfwApplication
{
    GLFWwindow *m_window = nullptr;
    unique_ptr<AppDelegate> m_appDelegate;
    unique_ptr<EngineController> m_engine;

public:
    glfwApplication(unique_ptr<AppDelegate> appDelegate)
        : m_appDelegate(std::move(appDelegate))
    {

    }

    ~glfwApplication()
    {
        if (m_window)
            glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void init()
    {
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
            throw std::runtime_error("Failed to create glfw window");
            glfwTerminate();
        }

        // Center the window.
        auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if (videoMode && !params.fullscreen)
            glfwSetWindowPos(m_window, (videoMode->width - params.windowWidth) / 2, (videoMode->height - params.windowHeight) / 2);

        glfwMakeContextCurrent(m_window);
        glfwSetWindowUserPointer(m_window, this);

        if (params.vsync)
            glfwSwapInterval(1);

        // Set input callbacks.
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetCharCallback(m_window, textInputCallback);
        glfwSetScrollCallback(m_window, scrollCallback);
        glfwSetCursorPosCallback(m_window, cursorPosCallback);

        m_engine.reset(new EngineController());
        m_engine->initialize(params);
        if (!m_appDelegate->onAppStarted())
            throw std::runtime_error("Game code initialization failed.");
    }

    void run()
    {
        using namespace std::chrono;

        while (!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();

            m_engine->step();

            glfwSwapBuffers(m_window);
        }

        m_appDelegate->onAppEnded();
        m_engine->shutdown();
        m_engine.reset();
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

    EngineController& getEngine() { return *m_engine; }
};

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    auto app = reinterpret_cast<glfwApplication*>(glfwGetWindowUserPointer(window));

    app->onMouseButton(button, action, mods);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto app = reinterpret_cast<glfwApplication*>(glfwGetWindowUserPointer(window));

    app->onKey(key, scancode, action, mods);
}

static void textInputCallback(GLFWwindow *window, unsigned int codepoint)
{
    auto app = reinterpret_cast<glfwApplication*>(glfwGetWindowUserPointer(window));

    app->onTextInput(codepoint);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    auto app = reinterpret_cast<glfwApplication*>(glfwGetWindowUserPointer(window));

    app->onScroll(xoffset, yoffset);
}

void cursorPosCallback(GLFWwindow *window, double x, double y)
{
    auto app = reinterpret_cast<glfwApplication*>(glfwGetWindowUserPointer(window));

    app->onCursorMove(x, y);
}

glfwApplication *g_application = nullptr;

void glfwAppRun()
{
    DF3D_ASSERT(g_application, "app must be initialized first!");

    g_application->run();

    delete g_application;
}

} }

namespace df3d {

void Application::setupDelegate(unique_ptr<AppDelegate> appDelegate)
{
    platform_impl::g_application = new platform_impl::glfwApplication(std::move(appDelegate));
    platform_impl::g_application->init();
}

void Application::setTitle(const std::string &title)
{
    DF3D_ASSERT(platform_impl::g_application, "failed to set title");
    platform_impl::g_application->setTitle(title);
}

EngineController& svc() { return platform_impl::g_application->getEngine(); }

}
