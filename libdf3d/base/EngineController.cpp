#include "EngineController.h"

#include "DebugConsole.h"
#include "TimeManager.h"
#include <render/RenderManager.h>
#include <3d/Camera.h>
#include <game/World.h>
#include <resources/ResourceManager.h>
#include <input/InputManager.h>
#include <io/FileSystem.h>
#include <gui/GuiManager.h>
#include <audio/AudioManager.h>
#include <render/RenderStats.h>
#include <render/RenderTargetScreen.h>
#include <platform/AppDelegate.h>
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

        // Create time manager.
        m_timeManager = make_unique<TimeManager>();

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
        m_world = World::newWorld();

        m_initialized = true;
        glog << "Engine initialized" << logmess;

        // FIXME: may have big delta. Client code is initilized next.
        m_timeManager->updateFrameTime();
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

    m_world.reset();
    m_debugConsole.reset();
    m_guiManager.reset();
    m_renderManager.reset();
    m_resourceManager.reset();
    m_fileSystem.reset();
    m_audioManager.reset();
    m_inputManager.reset();
    m_timeManager.reset();
}

void EngineController::step()
{
    m_timeManager->updateFrameTime();
    auto systemDelta = m_timeManager->getSystemFrameTimeDuration();
    auto gameDelta = m_timeManager->getGameFrameTimeDuration();

    // Update some engine subsystems.
    // TODO_ecs: this will be removed completely!!!
    m_resourceManager->poll();
    m_guiManager->getContext()->Update();

    // Update client code.
    m_timeManager->flushPendingWorkers();
    m_timeManager->updateListeners();
    m_world->update(systemDelta, gameDelta);

    // TODO: clean up each n secs.
    // TODO_ecs: cleaning up was here!
    m_timeManager->cleanInvalidListeners();

    // Run frame.
    m_renderManager->drawWorld(*m_world);

    // Clean step.
    m_inputManager->cleanStep();
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

void EngineController::replaceWorld(unique_ptr<World> w)
{
    m_world = std::move(w);
}

World& world()
{
    return svc().world();
}

void replaceWorld(unique_ptr<World> w)
{
    svc().replaceWorld(std::move(w));
}

}
