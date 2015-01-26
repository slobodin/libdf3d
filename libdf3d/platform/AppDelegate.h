#pragma once

#include <base/InputEvents.h>

namespace df3d { namespace platform {

class DF3D_DLL AppDelegate
{
public:
    AppDelegate() { }
    virtual ~AppDelegate() { }

    virtual bool onAppStarted(int windowWidth, int windowHeight) = 0;
    virtual void onAppEnded() = 0;
    virtual void onAppUpdate(float dt) = 0;

    virtual void onMouseButtonEvent(const base::MouseButtonEvent &mouseButtonEvent) = 0;
    virtual void onMouseMotionEvent(const base::MouseMotionEvent &mouseMotionEvent) = 0;
    virtual void onKeyUp(const base::KeyboardEvent::KeyCode &code) = 0;
    virtual void onKeyDown(const base::KeyboardEvent::KeyCode &code) = 0;
};

} }