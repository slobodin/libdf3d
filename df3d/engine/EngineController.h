#pragma once

#include "EngineInitParams.h"
#include <df3d/lib/NonCopyable.h>
#include <df3d/Common.h>
#include <glm/glm.hpp>

namespace df3d {

class RenderManager;
class ResourceManager;
class GuiManager;
class InputManager;
class ScriptManager;
class Timer;
class TimeManager;
class Viewport;
class World;
class Allocator;

class EngineController : NonCopyable
{
    friend bool EngineInit(EngineInitParams params);
    friend void EngineShutdown();

    EngineInitParams m_initParams;
    unique_ptr<RenderManager> m_renderManager;
    unique_ptr<ResourceManager> m_resourceManager;
    unique_ptr<GuiManager> m_guiManager;
    unique_ptr<InputManager> m_inputManager;
    unique_ptr<ScriptManager> m_scriptManager;
    unique_ptr<TimeManager> m_systemTimeManager;
    unique_ptr<Timer> m_timer;

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

    bool isInitialized() const { return m_initialized; }

    glm::vec2 getScreenSize() const;

    const EngineInitParams& getInitParams() const { return m_initParams; }

    RenderManager& renderManager() { return *m_renderManager; }
    ResourceManager& resourceManager() { return *m_resourceManager; }
    GuiManager& guiManager() { return *m_guiManager; }
    InputManager& inputManager() { return *m_inputManager; }
    Timer& timer() { return *m_timer; }
    TimeManager& systemTimeManager() { return *m_systemTimeManager; }
    ScriptManager& scripts() { return *m_scriptManager; }

    World& defaultWorld() { return world(); }
    World& world() { return *m_world; }
    void replaceWorld();
    void replaceWorld(const char *resourceFile);
    void deleteWorld();
};

EngineController& svc();

// TODO: move from here.
class MemoryManager
{
    static Allocator *m_defaultAllocator;
public:
    static void init();
    static void shutdown();

    static Allocator& allocDefault() { return *m_defaultAllocator; }
};

}
