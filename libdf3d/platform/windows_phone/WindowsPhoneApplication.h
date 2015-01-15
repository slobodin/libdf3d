#pragma once

#include "../Application.h"

namespace df3d { namespace platform {

class WindowsPhoneApplication : public Application
{
    friend class base::Controller;
    friend class Application;

    struct Impl;
    unique_ptr<Impl> m_pImpl;

    WindowsPhoneApplication(const AppInitParams &params);
    virtual ~WindowsPhoneApplication();

    bool pollEvents() override;
    void swapBuffers() override;

    void setTitle(const char *title) override;
};

} }