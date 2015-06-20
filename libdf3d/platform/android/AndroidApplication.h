#pragma once

#include "../Application.h"

namespace df3d { namespace platform {

class AndroidApplication : public Application
{
    struct Impl;
    unique_ptr<Impl> m_pImpl;

public:
    AndroidApplication(const AppInitParams &params, AppDelegate *appDelegate);
    ~AndroidApplication();

    virtual void run() override;
    virtual void setTitle(const char *title) override;
};

} }
