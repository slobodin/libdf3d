#include "EngineController.h"

#include "DebugConsole.h"
#include "TimeManager.h"
#include "StringTable.h"
#include <libdf3d/render/RenderManager.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/game/World.h>
#include <libdf3d/game/impl/WorldLoader.h>
#include <libdf3d/resources/ResourceManager.h>
#include <libdf3d/input/InputManager.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/gui/GuiManager.h>
#include <libdf3d/audio/AudioManager.h>
#include <libdf3d/platform/AppDelegate.h>
#include <libdf3d/script/ScriptManager.h>
#include <libdf3d/utils/JsonUtils.h>

#if defined(DF3D_WINDOWS)
#include <libdf3d/platform/windows/CrashHandler.h>
#endif

namespace df3d {

EngineController::EngineController()
{

}

EngineController::~EngineController()
{

}

void EngineController::initialize(EngineInitParams params)
{
    DF3D_ASSERT(!m_initialized, "engine controller already initialized");

    DFLOG_MESS("Initializing df3d engine, width %d, height %d", params.windowWidth, params.windowHeight);

    try
    {
        srand((unsigned int)time(0));

        // Create timer.
        m_timer = make_unique<Timer>();

#ifdef DF3D_WINDOWS
        platform_impl::CrashHandler::setup();
#endif

        // Init filesystem.
        m_fileSystem = make_unique<FileSystem>();

        // Init resource manager.
        m_resourceManager = make_unique<ResourceManager>();
        m_resourceManager->initialize();

        // Init render system.
        m_renderManager = make_unique<RenderManager>(params);
        m_renderManager->initialize();

        // Init GUI.
        m_guiManager = make_unique<GuiManager>();
        m_guiManager->initialize(params.windowWidth, params.windowHeight);

        // Init audio subsystem.
        m_audioManager = make_unique<AudioManager>();
        m_audioManager->initialize();

        // Create console.
        if (params.createConsole)
            m_debugConsole = make_unique<DebugConsole>();

        // Create input subsystem.
        m_inputManager = make_unique<InputManager>();

        // Create a blank world.
        replaceWorld();

        // Startup squirrel.
        m_scriptManager = make_unique<ScriptManager>();
        m_scriptManager->initialize();

        // Allow to a client to listen system time.
        m_systemTimeManager = make_unique<TimeManager>();

        m_initialized = true;
        DFLOG_MESS("Engine initialized");

        // FIXME: may have big delta. Client code is initialized next.
        m_timer->update();
    }
    catch (std::exception &e)
    {
        DFLOG_CRITICAL("Engine initialization failed due to %s", e.what());
        throw;
    }
}

void EngineController::shutdown()
{
    DF3D_ASSERT(m_initialized, "failed to shutdown the engine, it's not initialized");

    m_world->destroyWorld();
    m_world.reset();

    m_debugConsole.reset();

    m_audioManager->shutdown();
    m_scriptManager->shutdown();
    m_guiManager->shutdown();
    m_renderManager->shutdown();
    m_resourceManager->shutdown();

    m_systemTimeManager.reset();
    m_scriptManager.reset();
    m_guiManager.reset();
    m_renderManager.reset();
    m_resourceManager.reset();
    m_fileSystem.reset();
    m_audioManager.reset();
    m_inputManager.reset();
    m_timer.reset();
}

void EngineController::step()
{
    if (m_suspended)
        return;

    m_timer->update();

    // Update some engine subsystems.
    m_resourceManager->poll();
    m_guiManager->update();

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
    DF3D_ASSERT(m_initialized, "EngineController must be initialized");
    if (!m_suspended)
    {
        df3d::svc().resourceManager().suspend();
        df3d::svc().audioManager().suspend();

        m_suspended = true;
    }
}

void EngineController::resume()
{
    DF3D_ASSERT(m_initialized, "EngineController must be initialized");
    if (m_suspended)
    {
        df3d::svc().resourceManager().resume();
        df3d::svc().audioManager().resume();
        m_suspended = false;
    }
}

void EngineController::setStringTable(const std::string &stringTablePath)
{
    m_stringTable = make_unique<StringTable>(stringTablePath);
}

void EngineController::setFileSystem(unique_ptr<IFileSystem> fs)
{
    m_fileSystem = std::move(fs);
}

glm::vec2 EngineController::getScreenSize() const
{
    const auto &vp = m_renderManager->getViewport();
    return glm::vec2(vp.width(), vp.height());
}

void EngineController::replaceWorld()
{
    if (m_world)
        m_world->destroyWorld();
    m_world = unique_ptr<World>(new World());
}

void EngineController::replaceWorld(const std::string &resourceFile)
{
    replaceWorld();
    game_impl::WorldLoader::initWorld(resourceFile, *m_world);
}

World& world()
{
    return svc().world();
}

void replaceWorld()
{
    svc().replaceWorld();
}

void replaceWorld(const std::string &worldResource)
{
    svc().replaceWorld(worldResource);
}

}
