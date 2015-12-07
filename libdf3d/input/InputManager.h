#pragma once

namespace df3d 
{

class InputListener;
class TouchEvent;
class MouseWheelEvent;
class MouseMotionEvent;
class MouseButtonEvent;
class KeyboardEvent;

class DF3D_DLL InputManager : utils::NonCopyable
{
    friend class EngineController;

    struct InputSubscriber
    {
        bool valid = true;
        InputListener *listener = nullptr;
    };

    // NOTE: using list because no iterators or references are invalidated when push_back'ing.
    std::list<InputSubscriber> m_inputListeners;
    InputSubscriber* findSubscriber(InputListener *l);

    void cleanInvalidListeners();

public:
    InputManager() = default;
    ~InputManager() = default;

    void registerInputListener(InputListener *listener);
    void unregisterInputListener(InputListener *listener);

    void pauseInput(bool pause);

    // This should be called by the platform code only.
    // TODO: improve encapsulation!
    void onTouchEvent(const TouchEvent &touchEvent);
    void onMouseButtonEvent(const MouseButtonEvent &mouseButtonEvent);
    void onMouseMotionEvent(const MouseMotionEvent &mouseMotionEvent);
    void onMouseWheelEvent(const MouseWheelEvent &mouseWheelEvent);
    void onKeyUp(const KeyboardEvent &keyUpEvent);
    void onKeyDown(const KeyboardEvent &keyDownEvent);
    void onTextInput(unsigned int codepoint);
};

//! Subclasses will be automatically listening input manager.
class DF3D_DLL InputListener
{
    friend class InputManager;

    uint64_t m_id;

public:
    InputListener();
    virtual ~InputListener();

    virtual void onTouchEvent(const TouchEvent &touchEvent) { }
    virtual void onMouseButtonEvent(const MouseButtonEvent &mouseButtonEvent) { }
    virtual void onMouseMotionEvent(const MouseMotionEvent &mouseMotionEvent) { }
    virtual void onKeyUp(const KeyboardEvent &keyUpEvent) { }
    virtual void onKeyDown(const KeyboardEvent &keyDownEvent) { }
};

}
