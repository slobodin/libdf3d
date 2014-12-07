#pragma once

#include "../Application.h"

namespace df3d { namespace platform {

class WindowsApplication : public Application
{
    friend class base::Controller;
    friend class Application;

    struct Impl;
    unique_ptr<Impl> m_pImpl;

    WindowsApplication(const AppInitParams &params);
    virtual ~WindowsApplication();

    bool pollEvents() override;
    void swapBuffers() override;

    void setTitle(const char *title) override;
};

} }