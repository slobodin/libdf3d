#pragma once

#include <base/InputEvents.h>

FWD_MODULE_CLASS(base, EngineController)
FWD_MODULE_CLASS(render, RenderManager)

namespace df3d { namespace gui {

class DF3D_DLL GuiManager
{
    // Update, render, init and shutdown should be called only by EngineController and RenderManager.
    friend class base::EngineController;
    friend class render::RenderManager;

    GuiManager(int contextWidth, int contextHeight);
    ~GuiManager();

    void update(float dt);
    void render();

public:
    // CEGUI related stuff.
    void setResourceGroupDirectory(const char *resourceGroup, const char *directory);

    bool processMouseButtonEvent(const base::MouseButtonEvent &ev);
    bool processMouseMotionEvent(const base::MouseMotionEvent &ev);
    bool processMouseWheelEvent(const base::MouseWheelEvent &ev);
    bool processKeyDownEvent(const base::KeyboardEvent::KeyCode &code);
    bool processKeyUpEvent(const base::KeyboardEvent::KeyCode &code);
};

} }