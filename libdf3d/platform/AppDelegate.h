#pragma once

#include <input/InputEvents.h>
#include <base/EngineInitParams.h>

namespace df3d {

class DF3D_DLL AppDelegate
{
public:
    AppDelegate() { }
    virtual ~AppDelegate() { }

    virtual EngineInitParams getInitParams() const = 0;

    virtual bool onAppStarted() = 0;
    virtual void onAppEnded() = 0;

    virtual void onAppPaused() = 0;
    virtual void onAppResumed() = 0;
};

class DF3D_DLL Application final
{
public:
    static void setupDelegate(unique_ptr<AppDelegate> appDelegate);
    static void setTitle(const std::string &title);
};

}

// Client code must call Application::setupDelegate here.
extern "C" void df3dInitialized();
