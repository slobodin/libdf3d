#pragma once

#include <utils/Dict.h>

namespace df3d { namespace base {

extern const std::string CVAR_DEBUG_DRAW;
// TODO:
// More cvars.

class DF3D_DLL DebugConsole : utils::NonCopyable
{
    class ConsoleWindow;
    ConsoleWindow *m_menu = nullptr;

    std::unordered_map<std::string, int> m_consoleCommands;
    utils::Dict m_cvars;

public:
    DebugConsole();
    ~DebugConsole();

    bool isVisible() const;
    void show();
    void hide();
    void toggle();

    void registerCommand();
    void unregisterCommand();

    utils::Dict& getCVars() { return m_cvars; }
    const utils::Dict& getCVars() const { return m_cvars; }
};

} }
