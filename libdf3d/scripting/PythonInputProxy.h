#pragma once

namespace df3d { namespace scripting {

class ScriptManager;

class DF3D_DLL PythonMouseInputProxy : boost::noncopyable
{
    friend class ScriptManager;

    struct Impl;
    scoped_ptr<Impl> m_pImpl;

    ScriptManager *m_sm = nullptr;

    PythonMouseInputProxy(ScriptManager *sm, const char *pyFile);
    void setButtonEventListener(const char *pyFn);
    void setMotionEventListener(const char *pyFn);

public:
    ~PythonMouseInputProxy();

    void onMouseButtonEvent(const SDL_MouseButtonEvent &mouseButtonEvent);
    void onMouseMotionEvent(const SDL_MouseMotionEvent &mouseMotionEvent);
};

class DF3D_DLL PythonKeyboardInputProxy : boost::noncopyable
{
    friend class ScriptManager;

    struct Impl;
    scoped_ptr<Impl> m_pImpl;

    ScriptManager *m_sm = nullptr;

    PythonKeyboardInputProxy(ScriptManager *sm, const char *pyFile);
    void setKeyDownEventListener(const char *pyFn);
    void setKeyUpEventListener(const char *pyFn);

public:
    ~PythonKeyboardInputProxy();

    void onKeyDown(const SDL_KeyboardEvent &keyEvent);
    void onKeyUp(const SDL_KeyboardEvent &keyEvent);
};

} }