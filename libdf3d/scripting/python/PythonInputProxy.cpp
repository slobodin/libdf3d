#include "df3d_pch.h"
#include "PythonInputProxy.h"

#include <scripting/ScriptManager.h>

namespace df3d { namespace scripting {

#if defined(DF3D_USES_PYTHON)

typedef boost::function<void(Uint8, Uint32, Sint32, Sint32)> PyMouseButtonHanler;
typedef boost::function<void(Uint32, Sint32, Sint32)> PyMouseMotionHanler;
typedef boost::function<void(Sint32)> PyKeyInputHandler;

struct PythonMouseInputProxy::Impl
{
    std::string file;

    PyMouseButtonHanler mouseButtonHandler;
    PyMouseMotionHanler mouseMotionHandler;
};

PythonMouseInputProxy::PythonMouseInputProxy(ScriptManager *sm, const char *pyFile)
    : m_pImpl(new Impl()),
    m_sm(sm)
{
    m_pImpl->file = pyFile;
}

void PythonMouseInputProxy::setButtonEventListener(const char *pyFn)
{
    m_pImpl->mouseButtonHandler = m_sm->getPythonObject<PyMouseButtonHanler>(m_pImpl->file.c_str(), pyFn);
}

void PythonMouseInputProxy::setMotionEventListener(const char *pyFn)
{
    m_pImpl->mouseMotionHandler = m_sm->getPythonObject<PyMouseMotionHanler>(m_pImpl->file.c_str(), pyFn);
}

PythonMouseInputProxy::~PythonMouseInputProxy()
{

}

void PythonMouseInputProxy::onMouseButtonEvent(const SDL_MouseButtonEvent &mouseButtonEvent)
{
    if (!m_pImpl->mouseButtonHandler)
        return;

    try
    {
        m_pImpl->mouseButtonHandler(mouseButtonEvent.button, mouseButtonEvent.type, mouseButtonEvent.x, mouseButtonEvent.y);
    }
    catch (...) { m_sm->printError(); }
}

void PythonMouseInputProxy::onMouseMotionEvent(const SDL_MouseMotionEvent &mouseMotionEvent)
{
    if (!m_pImpl->mouseMotionHandler)
        return;

    try
    {
        m_pImpl->mouseMotionHandler(mouseMotionEvent.state, mouseMotionEvent.x, mouseMotionEvent.y);
    }
    catch (...) { m_sm->printError(); }
}

struct PythonKeyboardInputProxy::Impl
{
    std::string file;

    PyKeyInputHandler keyDownHandler, keyUpHandler;
};

PythonKeyboardInputProxy::PythonKeyboardInputProxy(ScriptManager *sm, const char *pyFile)
    : m_pImpl(new Impl()),
    m_sm(sm)
{
    m_pImpl->file = pyFile;
}

void PythonKeyboardInputProxy::setKeyDownEventListener(const char *pyFn)
{
    m_pImpl->keyDownHandler = m_sm->getPythonObject<PyKeyInputHandler>(m_pImpl->file.c_str(), pyFn);
}

void PythonKeyboardInputProxy::setKeyUpEventListener(const char *pyFn)
{
    m_pImpl->keyUpHandler = m_sm->getPythonObject<PyKeyInputHandler>(m_pImpl->file.c_str(), pyFn);
}

PythonKeyboardInputProxy::~PythonKeyboardInputProxy()
{

}

void PythonKeyboardInputProxy::onKeyDown(const SDL_KeyboardEvent &keyEvent)
{
    if (!m_pImpl->keyDownHandler)
        return;

    try
    {
        m_pImpl->keyDownHandler(keyEvent.keysym.sym);
    }
    catch (...) { m_sm->printError(); }
}

void PythonKeyboardInputProxy::onKeyUp(const SDL_KeyboardEvent &keyEvent)
{
    if (!m_pImpl->keyUpHandler)
        return;

    try
    {
        m_pImpl->keyUpHandler(keyEvent.keysym.sym);
    }
    catch (...) { m_sm->printError(); }
}

#endif

} }