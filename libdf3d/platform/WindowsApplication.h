#pragma once

#include "Application.h"

namespace df3d { namespace platform {

class WindowsApplication : public Application
{
    friend class base::Controller;
    struct Impl;
    unique_ptr<Impl> m_pImpl;

protected:
    WindowsApplication();
    virtual ~WindowsApplication();

    bool init(AppInitParams params) override;
    void shutdown() override;

    bool pollEvents() override;
    void swapBuffers() override;

    void setTitle(const char *title) override;
};

} }