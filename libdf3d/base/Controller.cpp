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
#include <scripting/lua/LuaScriptManager.h>
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
#if defined(DF3D_WINDOWS)
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

void Controller::shutdown()
{
    m_appDelegate->onAppEnded();

    SAFE_DELETE(m_appDelegate);
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

    particlesys::destroySparkEngine();

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

    try
    {
        srand((unsigned int)time(0));

        // Init filesystem.
        m_fileSystem = new resources::FileSystem();
        m_fileSystem->addSearchPath("data/");
        parseConfig(params);

        // Init application.
        platform::AppInitParams appParams;
        appParams.windowWidth = params.windowWidth;
        appParams.windowHeight = params.windowHeight;
        appParams.fullscreen = params.fullscreen;

        m_application = platform::Application::create(appParams);
        // Set up delegate.
        m_appDelegate = appDelegate;

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

        // Init scripting subsystem.
        m_scriptManager = new scripting::LuaScriptManager();

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

        // Send app started to the client.
        m_appDelegate->onAppStarted();
    }
    catch (std::exception &e)
    {
        base::glog << "Engine initialization failed due to" << e.what() << base::logcritical;
    }

    return m_initialized;
}

void Controller::run()
{
    using namespace std::chrono;
    if (!m_initialized)
        return;

    TimePoint currtime, prevtime;
    currtime = prevtime = system_clock::now();

    float lastFpsCheck = 0.0f;

    while (!m_quitRequested)
    {
        if (!m_application->pollEvents())
        {
            currtime = system_clock::now();

            auto dt = IntervalBetween(currtime, prevtime);

            update(dt);
            runFrame();

            prevtime = currtime;

            m_currentFPS = 1.f / dt;
            lastFpsCheck += dt;

            if (lastFpsCheck > 1.0f)        // every second
            {
                std::ostringstream os;
                os << "Frame time: " << dt << ", fps: " << m_currentFPS << ", draw calls: " << m_renderManager->getLastRenderStats().drawCalls;

                m_application->setTitle(os.str().c_str());
                lastFpsCheck = 0.0f;
            }
        }
    }

    shutdown();
}

void Controller::requestShutdown()
{
    m_quitRequested = true;
}

void Controller::update(float dt)
{
    // Update engine.
    m_timeElapsed = IntervalBetweenNowAnd(m_timeStarted);

    m_audioManager->update(dt);
    m_physics->update(dt);
    m_sceneManager->update(dt);
    m_guiManager->update(dt);
    m_renderManager->update(m_sceneManager->getCurrentScene());

    // Update user code.
    m_appDelegate->onAppUpdate(dt);

    // Clean up.
    m_sceneManager->cleanStep();
}

void Controller::runFrame()
{
    m_renderManager->onFrameBegin();

    m_renderManager->drawScene(m_sceneManager->getCurrentScene());
    m_renderManager->drawGUI();

    m_renderManager->onFrameEnd();

    if (m_application)
        m_application->swapBuffers();
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

void Controller::dispatchAppEvent(const platform::AppEvent &event)
{
    switch (event.type)
    {
    case platform::AppEvent::Type::QUIT:
        m_quitRequested = true;
        break;
    case platform::AppEvent::Type::MOUSE_MOTION:
        m_guiManager->processMouseMotionEvent(event.mouseMotion);
        m_appDelegate->onMouseMotionEvent(event.mouseMotion);
        break;
    case platform::AppEvent::Type::MOUSE_BUTTON:
        m_guiManager->processMouseButtonEvent(event.mouseButton);
        m_appDelegate->onMouseButtonEvent(event.mouseButton);
        break;
    case platform::AppEvent::Type::MOUSE_WHEEL:
        m_guiManager->processMouseWheelEvent(event.mouseWheel);
        break;
    case platform::AppEvent::Type::KEYBOARD:
        if (event.keyboard.state == KeyboardEvent::State::PRESSED)
        {
            m_guiManager->processKeyDownEvent(event.keyboard.keycode);
            m_appDelegate->onKeyDown(event.keyboard.keycode);
        }
        else if (event.keyboard.state == KeyboardEvent::State::RELEASED)
        {
            m_guiManager->processKeyUpEvent(event.keyboard.keycode);
            m_appDelegate->onKeyUp(event.keyboard.keycode);
        }

        break;
    default:
        break;
    }
}

const render::Viewport &Controller::getViewport() const
{
    return m_renderManager->getScreenRenderTarget()->getViewport();
}

void Controller::setViewport(const render::Viewport &newvp)
{
    m_renderManager->getScreenRenderTarget()->setViewport(newvp);
}

} }
