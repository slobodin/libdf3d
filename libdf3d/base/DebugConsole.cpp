#include "df3d_pch.h"
#include "DebugConsole.h"

#include <base/SystemsMacro.h>
#include <utils/Utils.h>

#include <Rocket/Core.h>

extern const char *ConsoleRml;
extern const char *ConsoleRcss;

namespace df3d { namespace base {

void IConsole::registerCommand(const ConsoleCommand &command)
{
    if (df3d::utils::contains_key(m_consoleCommands, command.name))
    {
        base::glog << "Console command with name" << command.name << "already registered" << base::logwarn;
        return;
    }

    m_consoleCommands.insert(std::make_pair(command.name, command));
}

void IConsole::unregisterCommand(const ConsoleCommand &command)
{
    auto found = m_consoleCommands.find(command.name);
    if (found != m_consoleCommands.end())
        m_consoleCommands.erase(found);
    else
        base::glog << "IConsole::unregisterCommand failed. No such command" << command.name << base::logwarn;
}

const std::string CVAR_DEBUG_DRAW = "df3d_debug_draw";

class DebugConsole::ConsoleWindow : public Rocket::Core::ElementDocument, public Rocket::Core::EventListener
{
    void ProcessEvent(Rocket::Core::Event &ev)
    {
        if (ev == "click")
        {
            if (ev.GetTargetElement()->GetId() == "submit_command")
            {
                auto command = ev.GetTargetElement()->GetAttribute<Rocket::Core::String>("value", "");

                m_parent->onConsoleInput(command.CString());
            }
        }
    }

public:
    DebugConsole *m_parent = nullptr;

    ConsoleWindow(const Rocket::Core::String& tag)
        : ElementDocument(tag)
    {
        SetInnerRML(ConsoleRml);

        auto styleSheet = Rocket::Core::Factory::InstanceStyleSheetString(ConsoleRcss);

        SetStyleSheet(styleSheet);
        styleSheet->RemoveReference();
    }
};

void DebugConsole::onConsoleInput(const std::string &str)
{
    if (str.empty())
        return;
}

DebugConsole::DebugConsole()
{
    Rocket::Core::Factory::RegisterElementInstancer("__debug_console_window", new Rocket::Core::ElementInstancerGeneric<ConsoleWindow>())->RemoveReference();
    m_menu = dynamic_cast<ConsoleWindow *>(g_guiManager->getContext()->CreateDocument("__debug_console_window"));
    m_menu->m_parent = this;
    m_menu->SetProperty("visibility", "hidden");
}

DebugConsole::~DebugConsole()
{
    if (m_menu)
        m_menu->RemoveReference();
    m_menu = nullptr;
}

bool DebugConsole::isVisible() const
{
    return false;
}

void DebugConsole::show()
{

}

void DebugConsole::hide()
{

}

} }

/*
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
*/
