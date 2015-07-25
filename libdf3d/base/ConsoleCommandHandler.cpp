#include "df3d_pch.h"
#include "ConsoleCommandHandler.h"

#include <base/SystemsMacro.h>
#include <Rocket/Core.h>

extern const char *ConsoleRml;
extern const char *ConsoleRcss;

namespace df3d { namespace base {

class ConsoleCommandHandler::ConsoleWindow : public Rocket::Core::ElementDocument, public Rocket::Core::EventListener
{
    void ProcessEvent(Rocket::Core::Event &ev)
    {
        if (ev == "click")
        {
            if (ev.GetTargetElement()->GetId() == "submit_command")
            {
                //submitConsoleCommand();
            }
        }
    }

public:
    ConsoleWindow(const Rocket::Core::String& tag)
        : ElementDocument(tag)
    {
        SetInnerRML(ConsoleRml);

        auto styleSheet = Rocket::Core::Factory::InstanceStyleSheetString(ConsoleRcss);

        SetStyleSheet(styleSheet);
        styleSheet->RemoveReference();
    }
};

ConsoleCommandHandler::ConsoleCommandHandler()
{
    Rocket::Core::Factory::RegisterElementInstancer("__debug_console_window", new Rocket::Core::ElementInstancerGeneric<ConsoleWindow>())->RemoveReference();
    m_menu = dynamic_cast<ConsoleWindow *>(g_guiManager->getContext()->CreateDocument("__debug_console_window"));
    m_menu->SetProperty("visibility", "hidden");
}

ConsoleCommandHandler::~ConsoleCommandHandler()
{
    if (m_menu)
        m_menu->RemoveReference();
    m_menu = nullptr;
}

bool ConsoleCommandHandler::isVisible() const
{
    return false;
}

void ConsoleCommandHandler::show()
{

}

void ConsoleCommandHandler::hide()
{

}

void ConsoleCommandHandler::toggle()
{

}

void ConsoleCommandHandler::registerCommand()
{

}

void ConsoleCommandHandler::unregisterCommand()
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