#pragma once

FWD_MODULE_CLASS(base, Controller)
FWD_MODULE_CLASS(base, MouseMotionEvent)
FWD_MODULE_CLASS(render, RenderManager)

namespace df3d { namespace gui {

class DF3D_DLL GuiManager
{
    // Update, render, init and shutdown should be called only by Controller and RenderManager.
    friend class base::Controller;
    friend class render::RenderManager;

    GuiManager();
    ~GuiManager();

    // These are only for controller and render manager.
    bool init(int contextWidth, int contextHeight);
    void shutdown();

    void update(float dt);
    void render();

    void processMouseButtonEvent(const SDL_MouseButtonEvent &ev);
    void processMouseMotionEvent(const base::MouseMotionEvent &ev);
    void processMouseWheelEvent(const SDL_MouseWheelEvent &ev);
    void processKeyDownEvent(const SDL_KeyboardEvent &ev);
    void processKeyUpEvent(const SDL_KeyboardEvent &ev);
    void processTextInputEvent(const SDL_TextInputEvent &ev);

public:

};

} }