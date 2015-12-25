#include "EngineController.h"

#include "DebugConsole.h"
#include "TimeManager.h"
#include <render/RenderManager.h>
#include <3d/Camera.h>
#include <game/World.h>
#include <game/impl/WorldLoader.h>
#include <resources/ResourceManager.h>
#include <input/InputManager.h>
#include <io/FileSystem.h>
#include <gui/GuiManager.h>
#include <audio/AudioManager.h>
#include <render/RenderStats.h>
#include <render/RenderTargetScreen.h>
#include <platform/AppDelegate.h>
#include <script/ScriptManager.h>
#include <utils/JsonUtils.h>

#if defined(DF3D_WINDOWS)
#include <platform/windows/CrashHandler.h>
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
    assert(!m_initialized);

    glog << "Initializing df3d engine" << logmess;

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

        // Init render system.
        m_renderManager = make_unique<RenderManager>(params);

        // Init GUI.
        m_guiManager = make_unique<GuiManager>(params.windowWidth, params.windowHeight);

        // Init audio subsystem.
        m_audioManager = make_unique<AudioManager>();

        // Load embedded resources.
        glog << "Loading embedded resources" << logdebug;

        m_renderManager->loadEmbedResources();

        // Create console.
        if (params.createConsole)
            m_debugConsole = make_unique<DebugConsole>();

        // Create input subsystem.
        m_inputManager = make_unique<InputManager>();

        // Create a blank world.
        replaceWorld();

        // Startup squirrel.
        m_scriptManager = make_unique<ScriptManager>();

        // Allow to client to listen system time.
        m_systemTimeManager = make_unique<TimeManager>();

        m_initialized = true;
        glog << "Engine initialized" << logmess;

        // FIXME: may have big delta. Client code is initialized next.
        m_timer->update();
    }
    catch (std::exception &e)
    {
        glog << "Engine initialization failed due to" << e.what() << logcritical;
        throw;
    }
}

void EngineController::shutdown()
{
    assert(m_initialized);

    m_world->destroyWorld();
    m_world.reset();
    m_systemTimeManager.reset();
    m_scriptManager.reset();
    m_debugConsole.reset();
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
    m_timer->update();

    // Update some engine subsystems.
    m_resourceManager->poll();
    m_guiManager->getContext()->Update();

    // Update client code.
    m_systemTimeManager->update();
    m_world->update();

    // Run frame.
    m_renderManager->drawWorld(*m_world);

    // Clean step for engine subsystems.
    m_inputManager->cleanStep();
    m_systemTimeManager->cleanStep();
}

const RenderStats& EngineController::getLastRenderStats() const
{
    return m_renderManager->getLastRenderStats();
}

const Viewport& EngineController::getViewport() const
{
    return m_renderManager->getScreenRenderTarget()->getViewport();
}

void EngineController::setViewport(const Viewport &newvp)
{
    m_renderManager->getScreenRenderTarget()->setViewport(newvp);
}

glm::vec2 EngineController::getScreenSize() const
{
    auto vp = m_renderManager->getScreenRenderTarget()->getViewport();
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
