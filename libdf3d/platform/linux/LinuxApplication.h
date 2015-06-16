#pragma once

#include "../Application.h"

namespace df3d { namespace platform {

class LinuxApplication : public Application
{
    struct Impl;
    unique_ptr<Impl> m_pImpl;

public:
    LinuxApplication(const AppInitParams &params, AppDelegate *appDelegate);
    virtual ~LinuxApplication();

    void run() override;
    void setTitle(const char *title) override;
};

} }
