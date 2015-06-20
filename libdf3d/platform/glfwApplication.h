#pragma once

#include "Application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace df3d { namespace platform {

class glfwApplication : public Application
{
    GLFWwindow *m_window = nullptr;

public:
    glfwApplication(const AppInitParams &params, AppDelegate *appDelegate);
    ~glfwApplication();

    virtual void run() override;
    virtual void setTitle(const char *title) override;
};

} }
