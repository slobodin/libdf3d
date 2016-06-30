#pragma once

#include "../Application.h"

namespace df3d { namespace platform {

class WindowsPhoneApplication : public Application
{
public:
    WindowsPhoneApplication(const AppInitParams &params, AppDelegate *appDelegate);
    virtual ~WindowsPhoneApplication();

    void run() override;
    void setTitle(const char *title) override;
};

} }