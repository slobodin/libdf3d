#pragma once

#include "EngineInitParams.h"

namespace df3d {

class RenderManager;
class ResourceManager;
class FileSystem;
class GuiManager;
class AudioManager;
class InputManager;
class ScriptManager;
class Timer;
class TimeManager;
class DebugConsole;
class RenderStats;
class Viewport;
class World;

class DF3D_DLL EngineController : utils::NonCopyable
{
    unique_ptr<RenderManager> m_renderManager;
    unique_ptr<ResourceManager> m_resourceManager;
    unique_ptr<FileSystem> m_fileSystem;
    unique_ptr<GuiManager> m_guiManager;
    unique_ptr<AudioManager> m_audioManager;
    unique_ptr<InputManager> m_inputManager;
    unique_ptr<ScriptManager> m_scriptManager;
    unique_ptr<TimeManager> m_systemTimeManager;
    unique_ptr<Timer> m_timer;

    unique_ptr<DebugConsole> m_debugConsole;

    unique_ptr<World> m_world;

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
    glm::vec2 getScreenSize() const;

    RenderManager& renderManager() { return *m_renderManager; }
    ResourceManager& resourceManager() { return *m_resourceManager; }
    FileSystem& fileSystem() { return *m_fileSystem; }
    GuiManager& guiManager() { return *m_guiManager; }
    AudioManager& audioManager() { return *m_audioManager; }
    InputManager& inputManager() { return *m_inputManager; }
    Timer& timer() { return *m_timer; }
    TimeManager& systemTimeManager() { return *m_systemTimeManager; }
    ScriptManager& scripts() { return *m_scriptManager; }
    DebugConsole* debugConsole() { return m_debugConsole.get(); }

    World& world() { return *m_world; }
    void replaceWorld();
    void replaceWorld(const std::string &resourceFile);
};

DF3D_DLL EngineController& svc();

// Shortcuts.
DF3D_DLL World& world();
DF3D_DLL void replaceWorld();
DF3D_DLL void replaceWorld(const std::string &worldResource);

}
