#include "GuiManager.h"

#include <tb_core.h>
#include <tb_debug.h>
#include <tb_system.h>
#include <tb_msg.h>
#include <animation/tb_widget_animation.h>
#include "TBInterface.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/input/InputEvents.h>

void register_tbbf_font_renderer();
void register_freetype_font_renderer();

namespace df3d {

GuiManager::GuiManager()
{

}

GuiManager::~GuiManager()
{

}

void GuiManager::initialize(int contextWidth, int contextHeight)
{
    DFLOG_MESS("Initializing turbobadger");

    m_width = contextWidth;
    m_height = contextHeight;

    m_renderer = gui_impl::CreateRenderer();

    tb::tb_core_init(m_renderer.get(), m_width, m_height);

    register_tbbf_font_renderer();
    register_freetype_font_renderer();

    tb::TBWidgetsAnimationManager::Init();

    replaceRoot();

    DFLOG_MESS("Device DPI: %d", tb::TBSystem::GetDPI());
}

void GuiManager::shutdown()
{
    tb::TBWidgetsAnimationManager::Shutdown();

    m_root.reset();

    tb::tb_core_shutdown();

    m_renderer.reset();
}

void GuiManager::update()
{
    tb::TBAnimationManager::Update();
    m_root->InvokeProcessStates();
    m_root->InvokeProcess();

    tb::TBMessageHandler::ProcessMessages();
}

void GuiManager::replaceRoot()
{
    if (!tb::tb_core_is_initialized())
    {
        DFLOG_WARN("Failed to replace GUI root. Context is not initialized");
        return;
    }

    m_root = make_unique<tb::TBWidget>();
    m_root->SetRect(tb::TBRect(0, 0, m_width, m_height));
}

void GuiManager::showDebugger()
{
#ifdef _DEBUG
    ShowDebugInfoSettingsWindow(m_root.get());
#endif
}

tb::TBRenderer* GuiManager::getRenderer()
{
    return tb::g_renderer;
}

tb::TBSkin* GuiManager::getSkin()
{
    return tb::g_tb_skin;
}

tb::TBWidgetsReader* GuiManager::getWidgetsReader()
{
    return tb::g_widgets_reader;
}

tb::TBLanguage* GuiManager::getLang()
{
    return tb::g_tb_lng;
}

tb::TBFontManager* GuiManager::getFontManager()
{
    return tb::g_font_manager;
}

}
