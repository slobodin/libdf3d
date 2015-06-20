#include "df3d_pch.h"
#include "glfwApplication.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace df3d { namespace platform {

base::KeyboardEvent::KeyCode convertKeyCode(int keyCode)
{
    switch(keyCode)
    {
    case GLFW_KEY_UP:
        return base::KeyboardEvent::KeyCode::UP;
    case GLFW_KEY_DOWN:
        return base::KeyboardEvent::KeyCode::DOWN;
    case GLFW_KEY_LEFT:
        return base::KeyboardEvent::KeyCode::LEFT;
    case GLFW_KEY_RIGHT:
        return base::KeyboardEvent::KeyCode::RIGHT;
    case GLFW_KEY_SPACE:
        return base::KeyboardEvent::KeyCode::SPACE;
    case GLFW_KEY_F1:
        return base::KeyboardEvent::KeyCode::F1;
    default:
        break;
    }

    return base::KeyboardEvent::KeyCode::UNDEFINED;
}

static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    auto appDelegate = reinterpret_cast<AppDelegate*>(glfwGetWindowUserPointer(window));

    base::MouseMotionEvent ev;
    ev.x = xpos;
    ev.y = ypos;
    ev.leftPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    ev.rightPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    appDelegate->onMouseMotionEvent(ev);
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    auto appDelegate = reinterpret_cast<AppDelegate*>(glfwGetWindowUserPointer(window));

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

    appDelegate->onMouseButtonEvent(ev);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto appDelegate = reinterpret_cast<AppDelegate*>(glfwGetWindowUserPointer(window));

    auto keyCode = convertKeyCode(key);

    if (action == GLFW_PRESS)
        appDelegate->onKeyDown(keyCode);
    else if (action == GLFW_RELEASE)
        appDelegate->onKeyUp(keyCode);
}

struct glfwApplication::Impl
{
    GLFWwindow *window = nullptr;
};

glfwApplication::glfwApplication(const AppInitParams &params, AppDelegate *appDelegate)
    : Application(params, appDelegate),
    m_pImpl(make_unique<Impl>())
{
    // Create window and OpenGL context.
    if (!glfwInit())
        throw std::runtime_error("Failed to init glfw");

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWmonitor *monitor = params.fullscreen ? glfwGetPrimaryMonitor() : nullptr;

    m_pImpl->window = glfwCreateWindow(params.windowWidth, params.windowHeight, "libdf3d_window", monitor, nullptr);
    if (!m_pImpl->window)
    {
        throw std::runtime_error("Failed to create glfw window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(m_pImpl->window);
    glfwSetWindowUserPointer(m_pImpl->window, m_appDelegate);

    // Set input callbacks.
    glfwSetCursorPosCallback(m_pImpl->window, cursorPositionCallback);
    glfwSetMouseButtonCallback(m_pImpl->window, mouseButtonCallback);
    glfwSetKeyCallback(m_pImpl->window, keyCallback);

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

    // Init user code.
    if (!m_appDelegate->onAppStarted(params.windowWidth, params.windowHeight))
        throw std::runtime_error("Game code initialization failed.");
}

glfwApplication::~glfwApplication()
{
    if (m_pImpl->window)
        glfwDestroyWindow(m_pImpl->window);
    glfwTerminate();
}

void glfwApplication::run()
{
    using namespace std::chrono;

    TimePoint currtime, prevtime;
    currtime = prevtime = system_clock::now();

    while (!glfwWindowShouldClose(m_pImpl->window))
    {
        currtime = system_clock::now();

        m_appDelegate->onAppUpdate(IntervalBetween(currtime, prevtime));

        glfwSwapBuffers(m_pImpl->window);

        prevtime = currtime;

        glfwPollEvents();
    }

    m_appDelegate->onAppEnded();
}

void glfwApplication::setTitle(const char *title)
{
    glfwSetWindowTitle(m_pImpl->window, title);
}

} }
