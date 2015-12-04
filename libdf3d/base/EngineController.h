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

    unique_ptr<DebugConsole> m_debugConsole;

    TimePoint m_timeStarted;
    float m_timeElapsed = 0;

public:
    EngineController();
    ~EngineController();

    // FIXME: using these instead ctor and dtor because of svc() access all over the engine code.
    void initialize(EngineInitParams params);
    void shutdown();

    void update(float systemDelta, float gameDelta);
    void postUpdate();
    void runFrame();

    float getElapsedTime() const { return m_timeElapsed; }
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
    DebugConsole* debugConsole() { return m_debugConsole.get(); }
};

DF3D_DLL EngineController& svc();

}
