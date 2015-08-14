#include "df3d_pch.h"

#include "../AppDelegate.h"
#include "glfwKeyCodes.h"
#include <GLFW/glfw3.h>

namespace df3d { namespace platform {

static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void textInputCallback(GLFWwindow *window, unsigned int codepoint);
static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

class glfwApplication
{
    GLFWwindow *window = nullptr;
    AppDelegate *m_appDelegate;

    // Touches emulation.
    unique_ptr<base::MouseMotionEvent> m_prevTouch;

public:
    glfwApplication(AppDelegate *appDelegate)
        : m_appDelegate(appDelegate)
    {
        // Create window and OpenGL context.
        if (!glfwInit())
            throw std::runtime_error("Failed to init glfw");

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        const auto params = m_appDelegate->getInitParams();

        GLFWmonitor *monitor = params.fullscreen ? glfwGetPrimaryMonitor() : nullptr;

        window = glfwCreateWindow(params.windowWidth, params.windowHeight, "libdf3d_window", monitor, nullptr);
        if (!window)
        {
            throw std::runtime_error("Failed to create glfw window");
            glfwTerminate();
        }

        // Center the window.
        auto videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if (videoMode && !params.fullscreen)
            glfwSetWindowPos(window, (videoMode->width - params.windowWidth) / 2, (videoMode->height - params.windowHeight) / 2);

        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);

        // Set input callbacks.
        glfwSetCursorPosCallback(window, cursorPositionCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCharCallback(window, textInputCallback);
        glfwSetScrollCallback(window, scrollCallback);

        // Init user code.
        if (!m_appDelegate->onAppStarted(params.windowWidth, params.windowHeight))
            throw std::runtime_error("Game code initialization failed.");
    }

    ~glfwApplication()
    {
        if (window)
            glfwDestroyWindow(window);
        glfwTerminate();

        delete m_appDelegate;
    }

    void run()
    {
        using namespace std::chrono;

        TimePoint currtime, prevtime;
        currtime = prevtime = system_clock::now();

        while (!glfwWindowShouldClose(window))
        {
            currtime = system_clock::now();

            m_appDelegate->onAppUpdate(IntervalBetween(currtime, prevtime));

            glfwSwapBuffers(window);

            prevtime = currtime;

            glfwPollEvents();
        }

        m_appDelegate->onAppEnded();
    }

    void setTitle(const std::string &title)
    {
        glfwSetWindowTitle(window, title.c_str());
    }

    void onMouseMove(float xpos, float ypos)
    {
        base::MouseMotionEvent ev;
        ev.x = xpos;
        ev.y = ypos;
        ev.leftPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        ev.rightPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

        if (m_prevTouch)
        {
            ev.dx = ev.x - m_prevTouch->x;
            ev.dy = ev.y - m_prevTouch->y;
        }
        else
        {
            m_prevTouch = make_unique<base::MouseMotionEvent>();
        }

        *m_prevTouch = ev;

        m_appDelegate->onMouseMotionEvent(ev);
    }

    void onMouseButton(int button, int action, int mods)
    {
        base::MouseButtonEvent ev;

        if (action == GLFW_PRESS)
            ev.state = base::MouseButtonEvent::State::PRESSED;
        else if (action == GLFW_RELEASE)
            ev.state = base::MouseButtonEvent::State::RELEASED;
        else
            return;

        if (button == GLFW_MOUSE_BUTTON_LEFT)
            ev.button = base::MouseButtonEvent::Button::LEFT;
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            ev.button = base::MouseButtonEvent::Button::RIGHT;
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
            ev.button = base::MouseButtonEvent::Button::MIDDLE;
        else
            return;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        ev.x = (int)xpos;
        ev.y = (int)ypos;

        m_appDelegate->onMouseButtonEvent(ev);
    }

    void onKey(int key, int scancode, int action, int mods)
    {
        auto keyCode = convertGlfwKeyCode(key);

        int keyModifiers = 0;
        if (mods & GLFW_MOD_SHIFT)
            keyModifiers |= base::KeyboardEvent::KM_SHIFT;
        if (mods & GLFW_MOD_ALT)
            keyModifiers |= base::KeyboardEvent::KM_ALT;
        if (mods & GLFW_MOD_CONTROL)
            keyModifiers |= base::KeyboardEvent::KM_CTRL;

        base::KeyboardEvent ev;
        ev.keycode = convertGlfwKeyCode(key);
        ev.modifiers = static_cast<base::KeyboardEvent::KeyModifier>(keyModifiers);

        if (action == GLFW_PRESS)
            m_appDelegate->onKeyDown(ev);
        else if (action == GLFW_RELEASE)
            m_appDelegate->onKeyUp(ev);
    }

    void onTextInput(unsigned int codepoint)
    {
        m_appDelegate->onTextInput(codepoint);
    }

    void onScroll(double xoffset, double yoffset)
    {
        base::MouseWheelEvent ev;
        ev.delta = (float)-yoffset;
        m_appDelegate->onMouseWheelEvent(ev);
    }
};

static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    auto app = reinterpret_cast<glfwApplication*>(glfwGetWindowUserPointer(window));

    app->onMouseMove((float)xpos, (float)ypos);
}

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

glfwApplication *g_application = nullptr;

void setupDelegate(AppDelegate *appDelegate)
{
    g_application = new glfwApplication(appDelegate);
}

void run()
{
    g_application->run();

    delete g_application;
}

void setTitle(const std::string &title)
{
    g_application->setTitle(title);
}

} }
