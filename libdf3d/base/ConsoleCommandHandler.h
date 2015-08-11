#pragma once

namespace df3d { namespace base {

class DF3D_DLL ConsoleCommandHandler : utils::NonCopyable
{
    class ConsoleWindow;
    ConsoleWindow *m_menu = nullptr;

    std::unordered_map<std::string, int> m_consoleCommands;

public:
    ConsoleCommandHandler();
    ~ConsoleCommandHandler();

    bool isVisible() const;
    void show();
    void hide();
    void toggle();

    void registerCommand();
    void unregisterCommand();
};

} }
