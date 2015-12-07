#pragma once

#include "EngineInitParams.h"

namespace df3d {

class RenderManager;
class SceneManager;
class ResourceManager;
class FileSystem;
class GuiManager;
class PhysicsManager;
class AudioManager;
class InputManager;
class TimeManager;
class DebugConsole;
class RenderStats;
class Viewport;
class AppDelegate;

class DF3D_DLL EngineController : utils::NonCopyable
{
    unique_ptr<RenderManager> m_renderManager;
    unique_ptr<SceneManager> m_sceneManager;
    unique_ptr<ResourceManager> m_resourceManager;
    unique_ptr<FileSystem> m_fileSystem;
    unique_ptr<GuiManager> m_guiManager;
    unique_ptr<PhysicsManager> m_physics;
    unique_ptr<AudioManager> m_audioManager;
    unique_ptr<InputManager> m_inputManager;
    unique_ptr<TimeManager> m_timeManager;

    unique_ptr<DebugConsole> m_debugConsole;

    bool m_initialized = false;

public:
    EngineController();
    ~EngineController();

    // FIXME: using these instead ctor and dtor because of svc() access all over the engine code.
    // These stuff should be called only by platform code. TODO: encapsulation improve.
    void initialize(EngineInitParams params);
    void shutdown();
    void step();

    const RenderStats& getLastRenderStats() const;
    const Viewport& getViewport() const;
    void setViewport(const Viewport &newvp);
    glm::vec2 getScreenSize() const;

    RenderManager& renderManager() { return *m_renderManager; }
    SceneManager& sceneManager() { return *m_sceneManager; }
    ResourceManager& resourceManager() { return *m_resourceManager; }
    FileSystem& fileSystem() { return *m_fileSystem; }
    GuiManager& guiManager() { return *m_guiManager; }
    PhysicsManager& physicsManager() { return *m_physics; }
    AudioManager& audioManager() { return *m_audioManager; }
    InputManager& inputManager() { return *m_inputManager; }
    TimeManager& timeManager() { return *m_timeManager; }
    DebugConsole* debugConsole() { return m_debugConsole.get(); }
};

DF3D_DLL EngineController& svc();

}
