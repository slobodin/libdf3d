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

    void processMouseButtonEvent(const base::MouseButtonEvent &ev);
    void processMouseMotionEvent(const base::MouseMotionEvent &ev);
    void processMouseWheelEvent(const base::MouseWheelEvent &ev);
    void processKeyDownEvent(const base::KeyboardEvent::KeyCode &code);
    void processKeyUpEvent(const base::KeyboardEvent::KeyCode &code);

public:
    // CEGUI related stuff.
    void setResourceGroupDirectory(const char *resourceGroup, const char *directory);
};

} }