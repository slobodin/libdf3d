#pragma once

#include <base/InputEvents.h>
#include <base/EngineInitParams.h>

namespace df3d { namespace platform {

class DF3D_DLL AppDelegate
{
public:
    AppDelegate() { }
    virtual ~AppDelegate() { }

    virtual base::EngineInitParams getInitParams() const = 0;

    virtual bool onAppStarted(int windowWidth, int windowHeight) = 0;
    virtual void onAppEnded() = 0;
    virtual void onAppUpdate(float dt) = 0;

    virtual void onAppPaused() = 0;
    virtual void onAppResumed() = 0;

    virtual void onTouchEvent(const base::TouchEvent &touchEvent) = 0;
    virtual void onMouseButtonEvent(const base::MouseButtonEvent &mouseButtonEvent) = 0;
    virtual void onMouseMotionEvent(const base::MouseMotionEvent &mouseMotionEvent) = 0;
    virtual void onMouseWheelEvent(const base::MouseWheelEvent &mouseWheelEvent) = 0;
    virtual void onKeyUp(const KeyboardEvent &keyboardEvent) = 0;
    virtual void onKeyDown(const KeyboardEvent &keyboardEvent) = 0;
    virtual void onTextInput(unsigned int codepoint) = 0;
};

void DF3D_DLL setupDelegate(AppDelegate *appDelegate);
void DF3D_DLL run();

void DF3D_DLL setTitle(const std::string &itle);

} }
