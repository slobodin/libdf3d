#pragma once

union SDL_Event;

FWD_MODULE_CLASS(render, RenderManager)
FWD_MODULE_CLASS(render, RenderStats)
FWD_MODULE_CLASS(render, Viewport)
FWD_MODULE_CLASS(scene, SceneManager)
FWD_MODULE_CLASS(resources, ResourceManager)
FWD_MODULE_CLASS(resources, FileSystem)
FWD_MODULE_CLASS(base, AppDelegate)
FWD_MODULE_CLASS(platform, Application)
FWD_MODULE_CLASS(gui, GuiManager)
FWD_MODULE_CLASS(gui, DebugOverlayWindow)
FWD_MODULE_CLASS(scripting, ScriptManager)
FWD_MODULE_CLASS(physics, PhysicsManager)
FWD_MODULE_CLASS(audio, AudioManager)

namespace df3d { namespace base {

struct DF3D_DLL EngineInitParams
{
    // All params will be gotten from the config file if this field is set.
    const char *configFile = nullptr;

    int argc = 0;
    char **argv = nullptr;

    int windowWidth = DEFAULT_WINDOW_WIDTH;
    int windowHeight = DEFAULT_WINDOW_HEIGHT;
    bool fullscreen = false;
    bool debugDraw = false;
    // TODO:
    // More params
    // More rendering params
    // Paths to the resources, etc
    // windowed, fullscreen mode
};

typedef std::function<std::string(const std::string &)> ConsoleCommandHandler;

class DF3D_DLL Controller : boost::noncopyable
{
    friend class gui::DebugOverlayWindow;

    Controller();
    ~Controller();

    platform::Application *m_application = nullptr;
    render::RenderManager *m_renderManager = nullptr;
    scene::SceneManager *m_sceneManager = nullptr;
    resources::ResourceManager *m_resourceManager = nullptr;
    resources::FileSystem *m_fileSystem = nullptr;
    gui::GuiManager *m_guiManager = nullptr;
    scripting::ScriptManager *m_scriptManager = nullptr;
    base::AppDelegate *m_appDelegate = nullptr;
    physics::PhysicsManager *m_physics = nullptr;
    audio::AudioManager *m_audioManager = nullptr;

    gui::DebugOverlayWindow *m_debugWindow = nullptr;

    std::unordered_map<std::string, ConsoleCommandHandler> m_consoleCommandsHandlers;
    void consoleCommandInvoked(const std::string &name, std::string &result);

    bool m_initialized = false;
    bool m_quitRequested = false;

    float m_currentFPS = 0;
    TimePoint m_timeStarted;
    float m_timeElapsed = 0;

    void parseConfig(EngineInitParams &config);

    void updateController(float dt);
    void runFrame();
    void shutdown();

public:
    static Controller *getInstance();

    bool init(EngineInitParams params, base::AppDelegate *appDelegate);
    void run();
    void requestShutdown();

    float getElapsedTime() const { return m_timeElapsed; }

    const render::RenderStats &getLastRenderStats() const;
    float getCurrentFPS() const { return m_currentFPS; }

    void toggleDebugWindow();

    void registerConsoleCommandHandler(const char *commandName, ConsoleCommandHandler handler);
    void unregisterConsoleCommandHandler(const char *commandName);

    bool initialized() const { return m_initialized; }

    // FIXME:
    // Should we use SDL everywhere?
    void dispatchAppEvent(SDL_Event *event);
    
    base::AppDelegate *getAppDelegate() { return m_appDelegate; }
    shared_ptr<render::Viewport> getViewport();

    scene::SceneManager *getSceneManager() { return m_sceneManager; }
    resources::ResourceManager *getResourceManager() { return m_resourceManager; }
    resources::FileSystem *getFileSystem() { return m_fileSystem; }
    render::RenderManager *getRenderManager() { return m_renderManager; }
    gui::GuiManager *getGuiManager() { return m_guiManager; }
    scripting::ScriptManager *getScriptManager() { return m_scriptManager; }
    physics::PhysicsManager *getPhysicsManager() { return m_physics; }
    audio::AudioManager *getAudioManager() { return m_audioManager; }
};

} }

#define g_engineController df3d::base::Controller::getInstance()

#define g_sceneManager df3d::base::Controller::getInstance()->getSceneManager()
#define g_resourceManager df3d::base::Controller::getInstance()->getResourceManager()
#define g_fileSystem df3d::base::Controller::getInstance()->getFileSystem()
#define g_renderManager df3d::base::Controller::getInstance()->getRenderManager()
#define g_guiManager df3d::base::Controller::getInstance()->getGuiManager()
#define g_scriptManager df3d::base::Controller::getInstance()->getScriptManager()
#define g_physicsWorld df3d::base::Controller::getInstance()->getPhysicsManager()->getWorld()
#define g_audioManager df3d::base::Controller::getInstance()->getAudioManager()