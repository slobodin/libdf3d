#pragma once

#include "EngineInitParams.h"

namespace df3d {

class Service;
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

class DF3D_DLL EngineController : utils::NonCopyable
{
    EngineController();
    ~EngineController();

    Service *m_svc = nullptr;
    RenderManager *m_renderManager = nullptr;
    SceneManager *m_sceneManager = nullptr;
    ResourceManager *m_resourceManager = nullptr;
    FileSystem *m_fileSystem = nullptr;
    GuiManager *m_guiManager = nullptr;
    PhysicsManager *m_physics = nullptr;
    AudioManager *m_audioManager = nullptr;

    DebugConsole *m_debugConsole = nullptr;

    bool m_initialized = false;

    TimePoint m_timeStarted;
    float m_timeElapsed = 0;

public:
    static EngineController& instance();

    bool init(EngineInitParams params);
    void shutdown();

    void update(float systemDelta, float gameDelta);
    void postUpdate();
    void runFrame();

    float getElapsedTime() const { return m_timeElapsed; }
    const RenderStats& getLastRenderStats() const;

    bool initialized() const { return m_initialized; }

    const Viewport& getViewport() const;
    void setViewport(const Viewport &newvp);
    glm::vec2 getScreenSize() const;

    Service& svc();
};

}
