#pragma once

#include <base/InputEvents.h>
#include <base/EngineInitParams.h>

namespace df3d {

class EngineController;

class DF3D_DLL AppDelegate
{
public:
    AppDelegate() { }
    virtual ~AppDelegate() { }

    virtual EngineInitParams getInitParams() const = 0;

    virtual bool onAppStarted() = 0;
    virtual void onAppEnded() = 0;
    virtual void onUpdate(float dt) = 0;

    virtual void onAppPaused() = 0;
    virtual void onAppResumed() = 0;

    virtual void onTouchEvent(const TouchEvent &touchEvent) = 0;
    virtual void onMouseButtonEvent(const MouseButtonEvent &mouseButtonEvent) = 0;
    virtual void onMouseMotionEvent(const MouseMotionEvent &mouseMotionEvent) = 0;
    virtual void onMouseWheelEvent(const MouseWheelEvent &mouseWheelEvent) = 0;
    virtual void onKeyUp(const KeyboardEvent &keyboardEvent) = 0;
    virtual void onKeyDown(const KeyboardEvent &keyboardEvent) = 0;
    virtual void onTextInput(unsigned int codepoint) = 0;
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
