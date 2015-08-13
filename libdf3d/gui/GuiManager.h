#pragma once

#include "RocketIntrusivePtr.h"
#include <base/InputEvents.h>

FWD_MODULE_CLASS(base, EngineController)
FWD_MODULE_CLASS(render, RenderManager)

namespace Rocket { namespace Core { class Context; } }

namespace df3d { namespace gui {

class GuiFileInterface;
class GuiSystemInterface;
class GuiRenderInterface;

class DF3D_DLL GuiManager
{
    // Update, render, init and shutdown should be called only by EngineController and RenderManager.
    friend class base::EngineController;
    friend class render::RenderManager;

    unique_ptr<GuiFileInterface> m_fileInterface;
    unique_ptr<GuiSystemInterface> m_systemInterface;
    unique_ptr<GuiRenderInterface> m_renderInterface;

    Rocket::Core::Context *m_rocketContext = nullptr;

    GuiManager(int contextWidth, int contextHeight);
    ~GuiManager();

    void update(float dt);
    void render();

public:
    // Calls RocketContext.
    bool processMouseButtonDown(int buttonIdx);
    bool processMouseButtonUp(int buttonIdx);
    bool processMouseMotion(int x, int y);
    bool processMouseWheel(int delta);
    bool processKeyDownEvent(const base::KeyboardEvent::KeyCode &code);
    bool processKeyUpEvent(const base::KeyboardEvent::KeyCode &code);
    bool processTextInput(unsigned int codepoint);

    RocketDocument loadDocument(const std::string &name);

    void showDebugger(bool show);
    bool isDebuggerVisible() const;

    Rocket::Core::Context *getContext() { return m_rocketContext; }
};

} }
