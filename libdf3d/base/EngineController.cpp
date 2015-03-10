#include "df3d_pch.h"
#include "EngineController.h"

#include "InputEvents.h"
#include <render/RenderManager.h>
#include <platform/Application.h>
#include <scene/SceneManager.h>
#include <scene/Camera.h>
#include <resources/ResourceManager.h>
#include <resources/FileSystem.h>
#include <gui/GuiManager.h>
#include <gui/DebugOverlayWindow.h>
#include <physics/PhysicsManager.h>
#include <audio/AudioManager.h>
#include <render/RenderStats.h>
#include <render/RenderTargetScreen.h>
#include <utils/JsonHelpers.h>
#include <particlesys/SparkInterface.h>

#if defined(DF3D_WINDOWS)
#define ENABLE_DEBUG_WINDOW
#endif

namespace df3d { namespace base {

EngineController::EngineController()
{
    base::glog << "Engine controller created" << base::logmess;

    m_timeStarted = std::chrono::system_clock::now();
}

EngineController::~EngineController()
{
}

void EngineController::consoleCommandInvoked(const std::string &name, std::string &result)
{
    if (name.empty())
        return;

    // Special case:
    if (name == "help")
    {
        result += "Registered commands: ";

        for (auto it : m_consoleCommandsHandlers)
            result += it.first + ", ";

        return;
    }

    auto space = name.find_first_of(' ');
    auto commandName = name.substr(0, space);

    auto found = m_consoleCommandsHandlers.find(commandName);
    if (found == m_consoleCommandsHandlers.end())
    {
        result = "No such command. Try help for more information.";
        return;
    }

    std::string params;
    if (space != std::string::npos)
        params = name.substr(space, std::string::npos);

    result = found->second(params);
}

void EngineController::shutdown()
{
    SAFE_DELETE(m_physics);
    SAFE_DELETE(m_sceneManager);
    SAFE_DELETE(m_guiManager);
    SAFE_DELETE(m_renderManager);
    SAFE_DELETE(m_resourceManager);
    SAFE_DELETE(m_fileSystem);
    SAFE_DELETE(m_audioManager);
    SAFE_DELETE(m_debugWindow);

    particlesys::destroySparkEngine();

    base::glog << "Shutdown success" << base::logmess;

    //delete this;
}

EngineController *EngineController::getInstance()
{
    static EngineController *controller = nullptr;
    if (!controller)
        controller = new EngineController();

    return controller;
}

bool EngineController::init(EngineInitParams params)
{
    base::glog << "Initializing engine" << base::logmess;

    try
    {
        srand((unsigned int)time(0));

        // Init filesystem.
        m_fileSystem = new resources::FileSystem();
        m_fileSystem->addSearchPath("data/");

        // Init resource manager.
        m_resourceManager = new resources::ResourceManager();

        // Init render system.
        render::RenderManagerInitParams renderParams;
        renderParams.viewportWidth = params.windowWidth;
        renderParams.viewportHeight = params.windowHeight;
        renderParams.debugDraw = params.debugDraw;
        m_renderManager = new render::RenderManager(renderParams);

        // Init scene manager.
        m_sceneManager = new scene::SceneManager();

        // Spark particle engine init.
        particlesys::initSparkEngine();

        // Init GUI.
        m_guiManager = new gui::GuiManager(params.windowWidth, params.windowHeight);

        // Init debug window.
#ifdef ENABLE_DEBUG_WINDOW
        m_debugWindow = new gui::DebugOverlayWindow();
#endif

        // Init physics.
        m_physics = new physics::PhysicsManager();

        // Init audio subsystem.
        m_audioManager = new audio::AudioManager();

        base::glog << "Engine initialized" << base::logmess;

        m_initialized = true;
    }
    catch (std::exception &e)
    {
        base::glog << "Engine initialization failed due to" << e.what() << base::logcritical;
    }

    return m_initialized;
}

void EngineController::update(float dt)
{
    // Update engine.
    m_timeElapsed = IntervalBetweenNowAnd(m_timeStarted);

    m_audioManager->update(dt);
    m_physics->update(dt);
    m_sceneManager->update(dt);
    m_guiManager->update(dt);
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

void EngineController::toggleDebugWindow()
{
#ifdef ENABLE_DEBUG_WINDOW
    m_debugWindow->toggle();
#endif
}

void EngineController::registerConsoleCommandHandler(const char *commandName, ConsoleCommandHandler handler)
{
    auto found = m_consoleCommandsHandlers.find(commandName);
    if (found != m_consoleCommandsHandlers.end())
    {
        base::glog << "Console command handler for" << commandName << "already registered" << base::logwarn;
        return;
    }

    m_consoleCommandsHandlers[commandName] = handler;
}

void EngineController::unregisterConsoleCommandHandler(const char *commandName)
{
    m_consoleCommandsHandlers.erase(commandName);
}

void EngineController::dispatchAppEvent(const platform::AppEvent &event)
{
    switch (event.type)
    {
    case platform::AppEvent::Type::MOUSE_MOTION:
        m_guiManager->processMouseMotionEvent(event.mouseMotion);
        break;
    case platform::AppEvent::Type::MOUSE_BUTTON:
        m_guiManager->processMouseButtonEvent(event.mouseButton);
        break;
    case platform::AppEvent::Type::MOUSE_WHEEL:
        m_guiManager->processMouseWheelEvent(event.mouseWheel);
        break;
    case platform::AppEvent::Type::KEYBOARD:
        if (event.keyboard.state == KeyboardEvent::State::PRESSED)
            m_guiManager->processKeyDownEvent(event.keyboard.keycode);
        else if (event.keyboard.state == KeyboardEvent::State::RELEASED)
            m_guiManager->processKeyUpEvent(event.keyboard.keycode);
        break;
    default:
        break;
    }
}

const render::Viewport &EngineController::getViewport() const
{
    return m_renderManager->getScreenRenderTarget()->getViewport();
}

void EngineController::setViewport(const render::Viewport &newvp)
{
    m_renderManager->getScreenRenderTarget()->setViewport(newvp);
}

} }
