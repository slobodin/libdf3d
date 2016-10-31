#pragma once

#include <df3d/engine/input/InputEvents.h>
#include <df3d/engine/EngineInitParams.h>

namespace df3d {

class AppDelegate
{
public:
    AppDelegate() = default;
    virtual ~AppDelegate() = default;

    virtual EngineInitParams getInitParams() const = 0;

    virtual bool onAppStarted() = 0;
    virtual void onAppEnded() = 0;

    virtual void onAppWillResignActive() = 0;
    virtual void onAppDidEnterBackground() = 0;

    virtual void onAppWillEnterForeground() = 0;
    virtual void onAppDidBecomeActive() = 0;

    virtual void onRenderDestroyed() = 0;
    virtual void onRenderRecreated() = 0;

    // Should be called by client code at entry point.
    static void setInstance(AppDelegate *app);
    static AppDelegate* getInstance();
};

class Application final
{
public:
    static void setTitle(const std::string &title);
};

}
