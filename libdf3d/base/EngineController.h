#pragma once

FWD_MODULE_CLASS(render, RenderManager)
FWD_MODULE_CLASS(render, RenderStats)
FWD_MODULE_CLASS(render, Viewport)
FWD_MODULE_CLASS(scene, SceneManager)
FWD_MODULE_CLASS(resources, ResourceManager)
FWD_MODULE_CLASS(resources, FileSystem)
FWD_MODULE_CLASS(gui, GuiManager)
FWD_MODULE_CLASS(physics, PhysicsManager)
FWD_MODULE_CLASS(audio, AudioManager)

#include "EngineInitParams.h"

namespace df3d { namespace base {

class IConsole;

class DF3D_DLL EngineController : utils::NonCopyable
{
    EngineController();
    ~EngineController();

    render::RenderManager *m_renderManager = nullptr;
    scene::SceneManager *m_sceneManager = nullptr;
    resources::ResourceManager *m_resourceManager = nullptr;
    resources::FileSystem *m_fileSystem = nullptr;
    gui::GuiManager *m_guiManager = nullptr;
    physics::PhysicsManager *m_physics = nullptr;
    audio::AudioManager *m_audioManager = nullptr;

    IConsole *m_debugConsole = nullptr;

    bool m_initialized = false;

    TimePoint m_timeStarted;
    float m_timeElapsed = 0;

public:
    static EngineController *getInstance();

    bool init(EngineInitParams params);
    void shutdown();

    void update(float systemDelta, float gameDelta);
    void postUpdate();
    void runFrame();

    float getElapsedTime() const { return m_timeElapsed; }
    const render::RenderStats &getLastRenderStats() const;

    bool initialized() const { return m_initialized; }

    const render::Viewport &getViewport() const;
    void setViewport(const render::Viewport &newvp);
    glm::vec2 screenSize() const;

    scene::SceneManager *getSceneManager() { return m_sceneManager; }
    resources::ResourceManager *getResourceManager() { return m_resourceManager; }
    resources::FileSystem *getFileSystem() { return m_fileSystem; }
    render::RenderManager *getRenderManager() { return m_renderManager; }
    gui::GuiManager *getGuiManager() { return m_guiManager; }
    physics::PhysicsManager *getPhysicsManager() { return m_physics; }
    audio::AudioManager *getAudioManager() { return m_audioManager; }
    IConsole *getConsole() { return m_debugConsole; }
};

} }
