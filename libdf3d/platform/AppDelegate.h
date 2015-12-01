#pragma once

#include <base/InputEvents.h>
#include <base/EngineInitParams.h>

namespace df3d { namespace platform {

class DF3D_DLL AppDelegate
{
public:
    AppDelegate() { }
    virtual ~AppDelegate() { }

    virtual EngineInitParams getInitParams() const = 0;

    virtual bool onAppStarted(int windowWidth, int windowHeight) = 0;
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

void DF3D_DLL setupDelegate(AppDelegate *appDelegate);
void DF3D_DLL setTitle(const std::string &itle);

} }
