#include "EngineController.h"

#include "TimeManager.h"
#include <df3d/engine/render/RenderManager.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/game/World.h>
#include <df3d/game/impl/WorldLoader.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/input/InputManager.h>
#include <df3d/engine/gui/GuiManager.h>
#include <df3d/platform/AppDelegate.h>
#include <df3d/engine/script/ScriptManager.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/memory/MallocAllocator.h>

#if defined(DF3D_WINDOWS)
#include <df3d/platform/windows/CrashHandler.h>
#endif

namespace df3d {

void EngineController::initialize(EngineInitParams params)
{
    DF3D_ASSERT_MESS(!m_initialized, "engine controller already initialized");

    DFLOG_MESS("Initializing df3d engine, width %d, height %d", params.windowWidth, params.windowHeight);

    m_initParams = params;

#ifdef DF3D_WINDOWS
    platform_impl::CrashHandler::setup();
#endif

    m_timer = make_unique<Timer>();
    m_systemTimeManager = make_unique<TimeManager>();
    m_resourceManager = make_unique<ResourceManager>();
    m_renderManager = make_unique<RenderManager>();
    m_guiManager = make_unique<GuiManager>();
    m_inputManager = make_unique<InputManager>();
    m_scriptManager = make_unique<ScriptManager>();

    m_resourceManager->initialize();
    m_renderManager->initialize(params);
    m_guiManager->initialize(params.windowWidth, params.windowHeight);
    m_scriptManager->initialize();

    if (params.createConsole)
    {

    }

    m_initialized = true;
    DFLOG_MESS("Engine initialized");

    // FIXME: may have big delta. Client code is initialized next.
    m_timer->update();
}

void EngineController::shutdown()
{
    DF3D_ASSERT_MESS(m_initialized, "failed to shutdown the engine, it's not initialized");

    if (m_world)
    {
        m_world->destroyWorld();
        m_world.reset();
    }

    m_scriptManager->shutdown();
    m_guiManager->shutdown();
    m_renderManager->shutdown();
    m_resourceManager->shutdown();

    m_systemTimeManager.reset();
    m_scriptManager.reset();
    m_guiManager.reset();
    m_renderManager.reset();
    m_resourceManager.reset();
    m_inputManager.reset();
    m_timer.reset();

    m_initialized = false;  // Is it safe to init it again?...
}

void EngineController::step()
{
    if (m_suspended)
        return;

    m_timer->update();

    // Update some engine subsystems.
    m_guiManager->update();
    m_resourceManager->poll();

    // Update client code.
    m_systemTimeManager->update(m_timer->getFrameDelta(TIME_CHANNEL_SYSTEM));
    m_world->update();

    // Run frame.
    m_renderManager->drawWorld(*m_world);

    // Clean step for engine subsystems.
    m_inputManager->cleanStep();
    m_systemTimeManager->cleanStep();
}

void EngineController::suspend()
{
    DF3D_ASSERT_MESS(m_initialized, "EngineController must be initialized");
    if (!m_suspended)
    {
        m_resourceManager->suspend();

        m_suspended = true;
    }
}

void EngineController::resume()
{
    DF3D_ASSERT_MESS(m_initialized, "EngineController must be initialized");
    if (m_suspended)
    {
        m_resourceManager->resume();

        m_suspended = false;
    }
}

glm::vec2 EngineController::getScreenSize() const
{
    const auto &vp = m_renderManager->getViewport();
    return glm::vec2(vp.width, vp.height);
}

void EngineController::replaceWorld()
{
    deleteWorld();

    m_world = unique_ptr<World>(new World());
}

void EngineController::replaceWorld(const char *resourceFile)
{
    replaceWorld();
    game_impl::WorldLoader::initWorld(resourceFile, *m_world);
}

void EngineController::deleteWorld()
{
    if (m_world)
    {
        m_world->destroyWorld();
        m_world.reset();
    }
}

static EngineController g_engine;

bool EngineInit(EngineInitParams params)
{
    try
    {
        g_engine.initialize(params);
        return true;
    }
    catch (std::exception &e)
    {
        DFLOG_CRITICAL("Engine initialization failed due to %s", e.what());
        return false;
    }
}

void EngineShutdown()
{
    g_engine.shutdown();
}

EngineController& svc()
{
    return g_engine;
}

Allocator *MemoryManager::m_defaultAllocator = nullptr;

void MemoryManager::init()
{
    m_defaultAllocator = new MallocAllocator();
}

void MemoryManager::shutdown()
{
    delete m_defaultAllocator;
    m_defaultAllocator = nullptr;
}

}
