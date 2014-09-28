#pragma once

#include "RocketIntrusivePtr.h"

namespace Rocket { namespace Core { class Context; class ElementDocument; } }

FWD_MODULE_CLASS(base, Controller)
FWD_MODULE_CLASS(base, MouseMotionEvent)
FWD_MODULE_CLASS(render, RenderManager)

namespace df3d { namespace gui {

class GuiFileInterface;
class GuiSystemInterface;
class GuiRenderInterface;

class DF3D_DLL GuiManager
{
    // Update, render, init and shutdown should be called only by Controller and RenderManager.
    friend class base::Controller;
    friend class render::RenderManager;

    scoped_ptr<GuiFileInterface> m_fileInterface;
    scoped_ptr<GuiSystemInterface> m_systemInterface;
    scoped_ptr<GuiRenderInterface> m_renderInterface;

    Rocket::Core::Context *m_rocketContext;

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
    RocketDocument loadDocument(const char *name);
    RocketDocument loadDocumentFromMemory(const std::string &data);
    void loadFont(const char *path);

    void setDebuggerVisible(bool visible);
    bool isDebuggerVisible();

    Rocket::Core::Context *getRocketContext();
};

} }