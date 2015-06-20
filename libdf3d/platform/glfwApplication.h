#pragma once

#include "Application.h"

namespace df3d { namespace platform {

class glfwApplication : public Application
{
    struct Impl;
    unique_ptr<Impl> m_pImpl;

public:
    glfwApplication(const AppInitParams &params, AppDelegate *appDelegate);
    ~glfwApplication();

    virtual void run() override;
    virtual void setTitle(const char *title) override;
};

} }
