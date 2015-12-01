#pragma once

namespace df3d {

//namespace impl { class ConsoleWindow; class StatsWindow; class SceneTreeWindow; }

//class DebugOverlayWindow : public Rocket::Core::EventListener
//{
//    friend class impl::ConsoleWindow;
//
//    Rocket::Core::ElementDocument *m_debugMenu = nullptr;
//    impl::StatsWindow *m_statsWnd = nullptr;
//    impl::ConsoleWindow *m_consoleWnd = nullptr;
//    impl::SceneTreeWindow *m_sceneTreeWnd = nullptr;
//
//    void ProcessEvent(Rocket::Core::Event &ev);
//    bool m_visible = false;
//
//    void createDebugMenu(Rocket::Core::Context *context);
//    void createDebugStatsWindow(Rocket::Core::Context *context);
//    void createConsoleWindow(Rocket::Core::Context *context);
//    void createSceneTreeWindow(Rocket::Core::Context *context);
//
//    void onCommandInvoked(const std::string &command, std::string &result);
//
//public:
//    DebugOverlayWindow();
//    ~DebugOverlayWindow();
//
//    void toggle();
//    void onRocketShutdown();
//
//    void onMouseButtonEvent(const SDL_MouseButtonEvent &mouseButtonEvent);
//};

class DebugOverlayWindow
{
public:
    DebugOverlayWindow() { }
    ~DebugOverlayWindow() { }

    void toggle() { }
};

}
