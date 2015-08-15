#include "df3d_pch.h"
#include "DebugConsole.h"

#include <base/SystemsMacro.h>
#include <utils/Utils.h>

#include <boost/algorithm/string.hpp>
#include <Rocket/Core.h>

extern const char *ConsoleRml;
extern const char *ConsoleRcss;

namespace df3d { namespace base {

const std::string CVAR_DEBUG_DRAW = "df3d_debug_draw";

class DebugConsole::ConsoleWindow : public Rocket::Core::ElementDocument, public Rocket::Core::EventListener
{
    RocketElement m_inputText;
    RocketElement m_historyElement;

    void ProcessEvent(Rocket::Core::Event &ev)
    {
        if (ev == "click")
        {
            if (ev.GetTargetElement()->GetId() == "submit_command")
            {
                auto command = m_inputText->GetAttribute<Rocket::Core::String>("value", "");

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

        m_inputText = GetElementById("console_input");
        m_historyElement = GetElementById("previous_commands");
    }

    void setHistory(const std::string &history)
    {
        m_historyElement->SetInnerRML(Rocket::Core::String(history.c_str()));
        m_inputText->SetAttribute("value", "");
    }
};

void DebugConsole::registerDefaultCommands()
{
    ConsoleCommand helpCommand;
    helpCommand.name = "help";
    helpCommand.help = "Displays help message.";
    helpCommand.handler = [this](const std::vector<std::string> &params)
    {
        std::string result = "Available console commands:\n";
        for (auto kv : m_consoleCommands)
            result += "  " + kv.first + ": " + kv.second.help + "\n";

        return result;
    };

    registerCommand(helpCommand);

    ConsoleCommand setCommand;
    setCommand.name = "set";
    setCommand.help = "Sets console variable. Usage: `set var val`";
    setCommand.handler = [this](const std::vector<std::string> &params)
    {
        if (params.size() != 2)
            return std::string("Invalid number of arguments passed to `set` command.");

        getCVars().set(params[0], params[1]);

        return params[0] + " was set to " + params[1];
    };

    registerCommand(setCommand);
}

void DebugConsole::onConsoleInput(const std::string &str)
{
    if (str.empty())
        return;

    auto space = str.find_first_of(' ');
    auto commandName = str.substr(0, space);
    if (commandName.empty())
        return;

    auto found = m_consoleCommands.find(commandName);
    if (found == m_consoleCommands.end())
    {
        updateHistory(std::string("No such command `") + commandName + "`. Try help for more information.");
        return;
    }

    std::string params;
    if (space != std::string::npos)
        params = str.substr(space, std::string::npos);

    boost::trim(params);

    updateHistory(found->second.handler(utils::split(params, ' ')));
}

void DebugConsole::updateHistory(const std::string &commandResult)
{
    if (commandResult.empty())
        return;

    m_history += commandResult + "\n";

    m_menu->setHistory(m_history);
}

DebugConsole::DebugConsole()
{
    Rocket::Core::Factory::RegisterElementInstancer("__debug_console_window", new Rocket::Core::ElementInstancerGeneric<ConsoleWindow>())->RemoveReference();
    m_menu = dynamic_cast<ConsoleWindow *>(g_guiManager->getContext()->CreateDocument("__debug_console_window"));
    m_menu->m_parent = this;
    m_menu->Hide();

    registerDefaultCommands();
}

DebugConsole::~DebugConsole()
{
    if (m_menu)
        m_menu->RemoveReference();
    m_menu = nullptr;
}

bool DebugConsole::isVisible() const
{
    return m_menu->IsVisible();
}

void DebugConsole::show()
{
    m_menu->Show();
}

void DebugConsole::hide()
{
    m_menu->Hide();
}

void DebugConsole::registerCommand(const ConsoleCommand &command)
{
    if (command.name.empty())
    {
        base::glog << "Command name should not be empty" << base::logwarn;
        return;
    }

    if (df3d::utils::contains_key(m_consoleCommands, command.name))
    {
        base::glog << "Console command with name" << command.name << "already registered" << base::logwarn;
        return;
    }

    m_consoleCommands.insert(std::make_pair(command.name, command));
}

void DebugConsole::unregisterCommand(const ConsoleCommand &command)
{
    auto found = m_consoleCommands.find(command.name);
    if (found != m_consoleCommands.end())
        m_consoleCommands.erase(found);
    else
        base::glog << "DebugConsole::unregisterCommand failed. No such command" << command.name << base::logwarn;
}

} }
