#include "EngineController.h"

#include "InputEvents.h"
#include "DebugConsole.h"
#include "Service.h"
#include <render/RenderManager.h>
#include <scene/SceneManager.h>
#include <scene/Camera.h>
#include <resources/ResourceManager.h>
#include <resources/FileSystem.h>
#include <gui/GuiManager.h>
#include <physics/PhysicsManager.h>
#include <audio/AudioManager.h>
#include <render/RenderStats.h>
#include <render/RenderTargetScreen.h>
#include <utils/JsonUtils.h>
#include <particlesys/SparkInterface.h>

#if defined(DF3D_WINDOWS)
#include <platform/windows/CrashHandler.h>
#endif

namespace df3d {

EngineController::EngineController()
{
    glog << "Engine controller created" << logmess;

    m_timeStarted = std::chrono::system_clock::now();
}

EngineController::~EngineController()
{

}

void EngineController::shutdown()
{
    SAFE_DELETE(m_debugConsole);
    SAFE_DELETE(m_sceneManager);
    SAFE_DELETE(m_physics);
    SAFE_DELETE(m_guiManager);
    SAFE_DELETE(m_renderManager);
    SAFE_DELETE(m_resourceManager);
    SAFE_DELETE(m_fileSystem);
    SAFE_DELETE(m_audioManager);

    SAFE_DELETE(m_svc);

    particlesys::destroySparkEngine();

    glog << "Shutdown success" << base::logmess;

    //delete this;
}

EngineController& EngineController::instance()
{
    static EngineController *controller = nullptr;
    if (!controller)
        controller = new EngineController();

    return *controller;
}

bool EngineController::init(EngineInitParams params)
{
    glog << "Initializing engine" << base::logmess;

    try
    {
        srand((unsigned int)time(0));

#ifdef DF3D_WINDOWS
        platform::CrashHandler::setup();
#endif

        // Init filesystem.
        m_fileSystem = new resources::FileSystem();

        // Init resource manager.
        m_resourceManager = new ResourceManager();

        // Init render system.
        render::RenderManagerInitParams renderParams;
        renderParams.viewportWidth = params.windowWidth;
        renderParams.viewportHeight = params.windowHeight;
        m_renderManager = new render::RenderManager(renderParams);

        // Init scene manager.
        m_sceneManager = new SceneManager();

        // Spark particle engine init.
        particlesys::initSparkEngine();

        // Init GUI.
        m_guiManager = new gui::GuiManager(params.windowWidth, params.windowHeight);

        // Init physics.
        m_physics = new physics::PhysicsManager();

        // Init audio subsystem.
        m_audioManager = new audio::AudioManager();

        // Init services.
        m_svc = new Service(*m_sceneManager, *m_resourceManager, *m_fileSystem,
                            *m_renderManager, *m_guiManager,
                            *m_physics, *m_physics->getWorld(), *m_audioManager);

        m_initialized = true;

        // Load embedded resources.
        glog << "Loading embedded resources" << logdebug;

        m_resourceManager->loadEmbedResources();
        m_renderManager->loadEmbedResources();

        // Create console.
        if (params.createConsole)
        {
            m_debugConsole = new DebugConsole();
            m_svc->console = m_debugConsole;
        }

        glog << "Engine initialized" << base::logmess;
    }
    catch (std::exception &e)
    {
        glog << "Engine initialization failed due to" << e.what() << base::logcritical;
    }

    return m_initialized;
}

void EngineController::update(float systemDelta, float gameDelta)
{
    // Update engine.
    m_timeElapsed = IntervalBetweenNowAnd(m_timeStarted);

    m_resourceManager->poll();
    m_audioManager->update(systemDelta);
    m_physics->update(gameDelta);
    m_sceneManager->update(gameDelta);
    m_guiManager->update(systemDelta);
    m_renderManager->update(m_sceneManager->getCurrentScene());
}

void EngineController::postUpdate()
{
    // Clean up.
    m_sceneManager->cleanStep();
}

void EngineController::runFrame()
{
    m_renderManager->onFrameBegin();

    m_renderManager->drawScene(m_sceneManager->getCurrentScene());
    m_renderManager->drawGUI();

    m_renderManager->onFrameEnd();
}

const render::RenderStats &EngineController::getLastRenderStats() const
{
    return m_renderManager->getLastRenderStats();
}

const render::Viewport &EngineController::getViewport() const
{
    return m_renderManager->getScreenRenderTarget()->getViewport();
}

void EngineController::setViewport(const render::Viewport &newvp)
{
    m_renderManager->getScreenRenderTarget()->setViewport(newvp);
}

glm::vec2 EngineController::getScreenSize() const
{
    auto vp = m_renderManager->getScreenRenderTarget()->getViewport();
    return glm::vec2(vp.width(), vp.height());
}

Service& EngineController::svc()
{
    return *m_svc;
}

}
