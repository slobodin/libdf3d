#pragma once

#include "../Application.h"

namespace df3d { namespace platform {

class WindowsApplication : public Application
{
    struct Impl;
    unique_ptr<Impl> m_pImpl;

public:
    WindowsApplication(const AppInitParams &params, AppDelegate *appDelegate);
    virtual ~WindowsApplication();

    void run() override;
    void setTitle(const char *title) override;
};

} }