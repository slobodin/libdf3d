#include "DebugConsole.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/gui/GuiManager.h>
#include <df3d/lib/Utils.h>

extern const char *ConsoleRml;
extern const char *ConsoleRcss;

namespace df3d {

const std::string CVAR_DEBUG_DRAW = "df3d_debug_draw";

// TODO_tb
/*
class DebugConsole::ConsoleWindow : public Rocket::Core::ElementDocument, public Rocket::Core::EventListener
{
    std::string m_prevCommand;

    void ProcessEvent(Rocket::Core::Event &ev)
    {
        if (ev == "click")
        {
            if (ev.GetTargetElement()->GetId() == "submit_command")
            {
                auto command = m_inputText->GetAttribute<Rocket::Core::String>("value", "");

                m_parent->onConsoleInput(m_prevCommand = command.CString());
            }
        }
        if (ev == "keyup")
        {
            auto identifier = ev.GetParameter<int>("key_identifier", -1);
            if (identifier == Rocket::Core::Input::KeyIdentifier::KI_RETURN)
            {
                GetElementById("submit_command")->Click();
            }
            else if (identifier == Rocket::Core::Input::KeyIdentifier::KI_UP)
            {
                if (!m_prevCommand.empty())
                    m_inputText->SetAttribute("value", Rocket::Core::String(m_prevCommand.c_str()));
            }
        }
    }

public:
    DebugConsole *m_parent = nullptr;

    RocketElement m_inputText;
    RocketElement m_historyElement;

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
};
*/
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

    utils::trim(params);

    updateHistory(found->second.handler(utils::split(params, ' ')));
}

void DebugConsole::updateHistory(const std::string &commandResult)
{
    /*
    if (!commandResult.empty())
    {
        m_history += commandResult + "\n";
        m_menu->m_historyElement->SetInnerRML(Rocket::Core::String(m_history.c_str()));
    }

    m_menu->m_inputText->SetAttribute("value", "");
    */
}

DebugConsole::DebugConsole()
{
    /*
    Rocket::Core::Factory::RegisterElementInstancer("__debug_console_window", new Rocket::Core::ElementInstancerGeneric<ConsoleWindow>())->RemoveReference();
    m_menu = svc().guiManager().createDocument<ConsoleWindow>("__debug_console_window");
    m_menu->m_parent = this;
    m_menu->Hide();
    */

    registerDefaultCommands();
}

DebugConsole::~DebugConsole()
{

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

void DebugConsole::registerCommand(const ConsoleCommand &command)
{
    if (command.name.empty())
    {
        DFLOG_WARN("Command name should not be empty");
        return;
    }

    if (utils::contains_key(m_consoleCommands, command.name))
    {
        DFLOG_WARN("Console command with name %s already registered", command.name.c_str());
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
        DFLOG_WARN("DebugConsole::unregisterCommand failed. No such command %s", command.name.c_str());
}

}
