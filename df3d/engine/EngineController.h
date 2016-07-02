#pragma once

#include "EngineInitParams.h"

namespace df3d {

class RenderManager;
class ResourceManager;
class IFileSystem;
class GuiManager;
class AudioManager;
class InputManager;
class ScriptManager;
class Timer;
class TimeManager;
class DebugConsole;
class Viewport;
class World;
class Allocator;

class DF3D_DLL EngineController : NonCopyable
{
    friend bool EngineInit(EngineInitParams params);
    friend void EngineShutdown();

    unique_ptr<RenderManager> m_renderManager;
    unique_ptr<ResourceManager> m_resourceManager;
    unique_ptr<IFileSystem> m_fileSystem;
    unique_ptr<GuiManager> m_guiManager;
    unique_ptr<AudioManager> m_audioManager;
    unique_ptr<InputManager> m_inputManager;
    unique_ptr<ScriptManager> m_scriptManager;
    unique_ptr<TimeManager> m_systemTimeManager;
    unique_ptr<Timer> m_timer;

    unique_ptr<DebugConsole> m_debugConsole;

    unique_ptr<World> m_world;  // TODO: don't hold worlds in the engine.

    bool m_initialized = false;
    bool m_suspended = false;

    void initialize(EngineInitParams params);
    void shutdown();

public:
    EngineController() = default;
    ~EngineController() = default;

    void step();

    void suspend();
    void resume();

    void setFileSystem(unique_ptr<IFileSystem> fs);

    bool isInitialized() const { return m_initialized; }

    glm::vec2 getScreenSize() const;

    RenderManager& renderManager() { return *m_renderManager; }
    ResourceManager& resourceManager() { return *m_resourceManager; }
    IFileSystem& fileSystem() { return *m_fileSystem; }
    GuiManager& guiManager() { return *m_guiManager; }
    AudioManager& audioManager() { return *m_audioManager; }
    InputManager& inputManager() { return *m_inputManager; }
    Timer& timer() { return *m_timer; }
    TimeManager& systemTimeManager() { return *m_systemTimeManager; }
    ScriptManager& scripts() { return *m_scriptManager; }
    DebugConsole* debugConsole() { return m_debugConsole.get(); }

    World& defaultWorld() { return world(); }
    World& world() { return *m_world; }
    void replaceWorld();
    void replaceWorld(const std::string &resourceFile);
    void deleteWorld();
};

DF3D_DLL EngineController& svc();

// TODO: move from here.
class DF3D_DLL MemoryManager
{
    static Allocator *m_defaultAllocator;

public:
    static void init();
    static void shutdown();

    static Allocator* allocDefault() { return m_defaultAllocator; }
};

}
