#include "EngineController.h"

#include "DebugConsole.h"
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

    m_timeStarted = std::chrono::system_clock::now();

    glog << "Initializing df3d engine" << logmess;

    try
    {
        srand((unsigned int)time(0));

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

    destroySparkEngine();
}

void EngineController::update(float systemDelta, float gameDelta)
{
    // Update engine.
    m_timeElapsed = IntervalBetweenNowAnd(m_timeStarted);

    m_resourceManager->poll();
    m_audioManager->update(systemDelta, gameDelta);
    m_physics->update(systemDelta, gameDelta);
    m_sceneManager->update(systemDelta, gameDelta);
    m_guiManager->getContext()->Update();
    m_renderManager->update(m_sceneManager->getCurrentScene());
}

void EngineController::postUpdate()
{
    // Clean up.
    m_sceneManager->cleanStep();
    m_inputManager->cleanInvalidListeners();
}

void EngineController::runFrame()
{
    m_renderManager->onFrameBegin();

    m_renderManager->drawScene(m_sceneManager->getCurrentScene());
    m_renderManager->drawGUI();

    m_renderManager->onFrameEnd();
}

const RenderStats &EngineController::getLastRenderStats() const
{
    return m_renderManager->getLastRenderStats();
}

const Viewport &EngineController::getViewport() const
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
