#include "df3d_pch.h"
#include "Controller.h"

#include "AppDelegate.h"
#include "InputEvents.h"
#include <render/RenderManager.h>
#include <platform/Application.h>
#include <scene/SceneManager.h>
#include <scene/Camera.h>
#include <resources/ResourceManager.h>
#include <resources/FileSystem.h>
#include <gui/GuiManager.h>
#include <gui/DebugOverlayWindow.h>
#include <scripting/ScriptManager.h>
#include <physics/PhysicsManager.h>
#include <audio/AudioManager.h>
#include <render/RenderStats.h>
#include <utils/JsonHelpers.h>
#include <particlesys/SparkInterface.h>

#include <platform/SDLApplication.h>

#if defined(__WINDOWS__)
#define ENABLE_DEBUG_WINDOW
#endif

namespace df3d { namespace base {

Controller::Controller()
{
    base::glog << "Engine controller created" << base::logmess;

    m_timeStarted = std::chrono::system_clock::now();
}

Controller::~Controller()
{
}

void Controller::consoleCommandInvoked(const std::string &name, std::string &result)
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

void Controller::parseConfig(EngineInitParams &config)
{
#if defined(__WINDOWS__)
    if (!config.configFile)
        return;

    auto data = utils::jsonLoadFromFile(config.configFile);
    if (data.empty())
        return;

    if (!data["width"].empty())
        config.windowWidth = data["width"].asUInt();
    if (!data["height"].empty())
        config.windowHeight = data["height"].asUInt();

    config.fullscreen = data["fullscreen"].asBool();
    config.debugDraw = data["debugdraw"].asBool();
#endif
}

void Controller::updateController(float dt)
{
    m_timeElapsed = IntervalBetweenNowAnd(m_timeStarted);

    if (m_sceneManager->getCamera())
        m_sceneManager->getCamera()->onUpdate(dt);
    m_audioManager->update(dt);
    m_physics->update(dt);
    m_sceneManager->update(dt);
    m_guiManager->update(dt);
    m_renderManager->update(m_sceneManager->getCurrentScene());
}

void Controller::runFrame()
{
    m_renderManager->onFrameBegin();

    m_renderManager->drawScene(m_sceneManager->getCurrentScene());
    m_renderManager->drawGUI();

    m_renderManager->onFrameEnd();
}

void Controller::shutdown()
{
    m_appDelegate->onAppEnded();

    if (m_resourceManager)
        m_resourceManager->shutdown();
    if (m_audioManager)
        m_audioManager->shutdown();
    if (m_physics)
        m_physics->shutdown();
    if (m_renderManager)
        m_renderManager->shutdown();
    if (m_scriptManager)
        m_scriptManager->shutdown();
    if (m_sceneManager)
        m_sceneManager->shutdown();
    if (m_guiManager)
        m_guiManager->shutdown();
    if (m_application)
        m_application->shutdown();

    particlesys::destroySparkEngine();

    SAFE_DELETE(m_physics);
    SAFE_DELETE(m_scriptManager);
    SAFE_DELETE(m_sceneManager);
    SAFE_DELETE(m_guiManager);
    SAFE_DELETE(m_renderManager);
    SAFE_DELETE(m_resourceManager);
    SAFE_DELETE(m_application);
    SAFE_DELETE(m_fileSystem);
    SAFE_DELETE(m_audioManager);
    SAFE_DELETE(m_debugWindow);

    base::glog << "Shutdown success" << base::logmess;

    //delete this;
}

Controller *Controller::getInstance()
{
    static Controller *controller = nullptr;
    if (!controller)
        controller = new Controller();

    return controller;
}

bool Controller::init(EngineInitParams params, base::AppDelegate *appDelegate)
{
    base::glog << "Initializing engine" << base::logmess;

    srand((unsigned int)time(0));

    // Init filesystem.
    m_fileSystem = new resources::FileSystem();
    m_fileSystem->addSearchPath("data/");

    m_appDelegate = appDelegate;

#if defined(__WINDOWS__) || defined(__ANDROID__)
    m_application = new platform::SDLApplication();
#else
#error "Unsupported platform"
#endif

    parseConfig(params);

    platform::AppInitParams appParams;
    appParams.windowWidth = params.windowWidth;
    appParams.windowHeight = params.windowHeight;
    appParams.fullscreen = params.fullscreen;

    // Init application.
    if (!m_application->init(appParams))
        return false;

    // Init resource manager.
    m_resourceManager = new resources::ResourceManager();
    if (!m_resourceManager->init())
        return false;

    // Init render system.
    m_renderManager =  new render::RenderManager();
    render::RenderManagerInitParams renderParams;
    renderParams.viewportWidth = params.windowWidth;
    renderParams.viewportHeight = params.windowHeight;
    renderParams.debugDraw = params.debugDraw;
    if (!m_renderManager->init(renderParams))
        return false;

    // Init scene manager.
    m_sceneManager = new scene::SceneManager();
    if (!m_sceneManager->init())
        return false;

    // Spark particle engine init.
    particlesys::initSparkEngine();

    // Init scripting subsystem.
    m_scriptManager = new scripting::ScriptManager();
    if (!m_scriptManager->init())
        return false;

    // Init GUI.
    m_guiManager = new gui::GuiManager();
    if (!m_guiManager->init(params.windowWidth, params.windowHeight))
        return false;

    // Init debug window.
#ifdef ENABLE_DEBUG_WINDOW
    m_debugWindow = new gui::DebugOverlayWindow();
#endif

    // Init physics.
    m_physics = new physics::PhysicsManager();
    if (!m_physics->init())
        return false;

    // Init audio subsystem.
    m_audioManager = new audio::AudioManager();
    if (!m_audioManager->init())
        return false;

    base::glog << "Engine initialized" << base::logmess;

    m_initialized = true;

    // Send app started to the client.
    m_appDelegate->onAppStarted();

    return m_initialized;
}

void Controller::run()
{
    using namespace std::chrono;
    if (!m_initialized)
        return;

    TimePoint currtime, prevtime;
    currtime = prevtime = system_clock::now();

    static float lastFpsCheck = 0.0f;

    while (!m_quitRequested)
    {
        currtime = system_clock::now();

        auto dt = IntervalBetween(currtime, prevtime);

        m_application->pollEvents();

        // Update user code.
        m_appDelegate->onAppUpdate(dt);

        // Update engine.
        updateController(dt);

        // Render frame.
        runFrame();
        m_application->swapBuffers();

        // Clean up.
        m_sceneManager->cleanStep();

        prevtime = currtime;

        m_currentFPS = 1.f / dt;
        lastFpsCheck += dt;

        if (lastFpsCheck > 1.0f)        // every second
        {
            std::ostringstream os;
            os << "Frame time: " << dt << ", fps: " << m_currentFPS << ", draw calls: " << m_renderManager->getLastRenderStats().drawCalls;

            m_application->setTitle(os.str().c_str());
            lastFpsCheck = 0.0f;

            //base::glog << "FPS" << m_currentFPS << "frame time" << dt << base::logdebug;
        }
    }

    shutdown();
}

void Controller::requestShutdown()
{
    m_quitRequested = true;
}

const render::RenderStats &Controller::getLastRenderStats() const
{
    return m_renderManager->getLastRenderStats();
}

void Controller::toggleDebugWindow()
{
#ifdef ENABLE_DEBUG_WINDOW
    m_debugWindow->toggle();
#endif
}

void Controller::registerConsoleCommandHandler(const char *commandName, ConsoleCommandHandler handler)
{
    auto found = m_consoleCommandsHandlers.find(commandName);
    if (found != m_consoleCommandsHandlers.end())
    {
        base::glog << "Console command handler for" << commandName << "already registered" << base::logwarn;
        return;
    }

    m_consoleCommandsHandlers[commandName] = handler;
}

void Controller::unregisterConsoleCommandHandler(const char *commandName)
{
    m_consoleCommandsHandlers.erase(commandName);
}

void Controller::dispatchAppEvent(SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_QUIT:
        m_quitRequested = true;
        break;
    case SDL_KEYUP:
        m_guiManager->processKeyUpEvent(event->key);
        m_appDelegate->onKeyUp(event->key);
        if (m_sceneManager->getCamera())
            m_sceneManager->getCamera()->onKeyUp(event->key);
        break;
    case SDL_KEYDOWN:
        m_guiManager->processKeyDownEvent(event->key);
        m_appDelegate->onKeyDown(event->key);
        if (m_sceneManager->getCamera())
            m_sceneManager->getCamera()->onKeyDown(event->key);
        break;
    case SDL_TEXTINPUT:
        m_guiManager->processTextInputEvent(event->text);
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        m_guiManager->processMouseButtonEvent(event->button);
#ifdef ENABLE_DEBUG_WINDOW
        m_debugWindow->onMouseButtonEvent(event->button);
#endif
        m_appDelegate->onMouseButtonEvent(event->button);
        break;
    case SDL_MOUSEMOTION:
        m_guiManager->processMouseMotionEvent(event->motion);
        m_appDelegate->onMouseMotionEvent(event->motion);
        if (m_sceneManager->getCamera())
            m_sceneManager->getCamera()->onMouseMotionEvent(event->motion);
        break;
    case SDL_MOUSEWHEEL:
        m_guiManager->processMouseWheelEvent(event->wheel);
        break;
    case SDL_FINGERMOTION:
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
        m_appDelegate->onFingerEvent(event->tfinger);
        break;
    default:
        break;
    }
}

shared_ptr<render::Viewport> Controller::getViewport()
{
    return m_renderManager->getViewport();
}

} }
