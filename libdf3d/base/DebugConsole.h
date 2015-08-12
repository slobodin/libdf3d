#pragma once

#include <utils/Dict.h>

namespace df3d { namespace base {

extern const std::string CVAR_DEBUG_DRAW;
// TODO:
// More cvars.

struct DF3D_DLL ConsoleCommand
{
    std::string name;
    std::string help;
    std::function<void()> handler;
};

class DF3D_DLL IConsole : utils::NonCopyable
{
protected:
    utils::Dict m_cvars;
    std::unordered_map<std::string, ConsoleCommand> m_consoleCommands;

public:
    IConsole() = default;
    virtual ~IConsole() = default;

    virtual bool isVisible() const = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    void toggle() { isVisible() ? hide() : show(); }

    void registerCommand(const ConsoleCommand &command);
    void unregisterCommand(const ConsoleCommand &command);

    utils::Dict& getCVars() { return m_cvars; }
    const utils::Dict& getCVars() const { return m_cvars; }
};

class DF3D_DLL DebugConsole : public IConsole
{
    class ConsoleWindow;
    ConsoleWindow *m_menu = nullptr;
    friend class ConsoleWindow;

    void onConsoleInput(const std::string &str);

public:
    DebugConsole();
    ~DebugConsole();

    bool isVisible() const override;
    void show() override;
    void hide() override;
};

class DF3D_DLL NullConsole : public IConsole
{
public:
    bool isVisible() const override { return false; }
    void show() override { }
    void hide() override { }
};

} }
