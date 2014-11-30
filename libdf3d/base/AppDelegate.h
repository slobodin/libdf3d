#pragma once

#include "InputEvents.h"

namespace df3d { namespace base {

class AppDelegate
{
public:
    AppDelegate() { }
    virtual ~AppDelegate() { }

    virtual void onAppStarted() = 0;
    virtual void onAppEnded() = 0;

    virtual void onAppUpdate(float dt) = 0;
    virtual void onMouseButtonEvent(const MouseButtonEvent &mouseButtonEvent) = 0;
    virtual void onMouseMotionEvent(const MouseMotionEvent &mouseMotionEvent) = 0;
    virtual void onKeyUp(const base::KeyboardEvent::KeyCode &code) = 0;
    virtual void onKeyDown(const base::KeyboardEvent::KeyCode &code) = 0;
};

} }