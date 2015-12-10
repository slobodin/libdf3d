#include "EngineController.h"

#include "DebugConsole.h"
#include "TimeManager.h"
#include <render/RenderManager.h>
#include <scene/SceneManager.h>
#include <scene/Camera.h>
#include <resources/ResourceManager.h>
#include <input/InputManager.h>
#include <io/FileSystem.h>
#include <gui/GuiManager.h>
#include <physics/PhysicsManager.h>
#include <audio/AudioManager.h>
#include <render/RenderStats.h>
#include <render/RenderTargetScreen.h>
#include <platform/AppDelegate.h>
#include <utils/JsonUtils.h>
#include <particlesys/SparkInterface.h>

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

        // Init scene manager.
        m_sceneManager = make_unique<SceneManager>();

        // Spark particle engine init.
        initSparkEngine();

        // Init GUI.
        m_guiManager = make_unique<GuiManager>(params.windowWidth, params.windowHeight);

        // Init physics.
        m_physics = make_unique<PhysicsManager>();

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

    m_debugConsole.reset();
    m_sceneManager.reset();
    m_physics.reset();
    m_guiManager.reset();
    m_renderManager.reset();
    m_resourceManager.reset();
    m_fileSystem.reset();
    m_audioManager.reset();
    m_inputManager.reset();
    m_timeManager.reset();

    destroySparkEngine();
}

void EngineController::step()
{
    // Update engine.
    m_timeManager->updateFrameTime();
    auto systemDelta = m_timeManager->getSystemFrameTimeDuration();
    auto gameDelta = m_timeManager->getGameFrameTimeDuration();

    m_resourceManager->poll();
    m_audioManager->update(systemDelta, gameDelta);
    m_physics->update(systemDelta, gameDelta);
    m_sceneManager->update(systemDelta, gameDelta);
    m_guiManager->getContext()->Update();
    m_renderManager->update(m_sceneManager->getCurrentScene());

    // Update client code.
    m_timeManager->flushPendingWorkers();
    m_timeManager->updateListeners();

    // Clean step.
    // TODO: clean up each n secs.
    m_sceneManager->cleanStep();
    m_inputManager->cleanInvalidListeners();
    m_timeManager->cleanInvalidListeners();

    // Run frame.
    m_renderManager->onFrameBegin();
    m_renderManager->drawScene(m_sceneManager->getCurrentScene());
    m_renderManager->drawGUI();
    m_renderManager->onFrameEnd();
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

}
