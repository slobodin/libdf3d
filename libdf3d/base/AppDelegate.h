#pragma once

struct SDL_MouseMotionEvent;
struct SDL_TouchFingerEvent;
struct SDL_MouseButtonEvent;
struct SDL_KeyboardEvent;

namespace df3d { namespace base {

class AppDelegate
{
public:
    AppDelegate() { }
    virtual ~AppDelegate() { }

    virtual void onAppStarted() = 0;
    virtual void onAppEnded() = 0;

    virtual void onAppUpdate(float dt) = 0;
    virtual void onMouseButtonEvent(const SDL_MouseButtonEvent &mouseButtonEvent) = 0;
    virtual void onMouseMotionEvent(const SDL_MouseMotionEvent &mouseMotionEvent) = 0;
    virtual void onFingerEvent(const SDL_TouchFingerEvent &fingerEvent) = 0;
    virtual void onKeyUp(const SDL_KeyboardEvent &keyEvent) = 0;
    virtual void onKeyDown(const SDL_KeyboardEvent &keyEvent) = 0;
};

} }