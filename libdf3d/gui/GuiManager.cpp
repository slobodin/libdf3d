#include "GuiManager.h"

#include <tb_core.h>
#include <tb_debug.h>
#include <animation/tb_widget_animation.h>
#include "impl/TBInterface.h"
#include <libdf3d/base/EngineController.h>
#include <libdf3d/input/InputEvents.h>

namespace df3d {

GuiManager::GuiManager()
{

}

GuiManager::~GuiManager()
{

}

void GuiManager::initialize(int contextWidth, int contextHeight)
{
    glog << "Initializing turbobadger" << logmess;

    m_width = contextWidth;
    m_height = contextHeight;

    tb::tb_core_init(gui_impl::CreateRenderer());

    void register_tbbf_font_renderer();
    register_tbbf_font_renderer();

    tb::TBWidgetsAnimationManager::Init();

    replaceRoot();
}

void GuiManager::shutdown()
{
    tb::TBWidgetsAnimationManager::Shutdown();

    m_root.reset();

    tb::tb_core_shutdown();
}

void GuiManager::update()
{
    tb::TBAnimationManager::Update();
    m_root->InvokeProcessStates();
    m_root->InvokeProcess();
}

void GuiManager::replaceRoot()
{
    if (!tb::tb_core_is_initialized())
    {
        glog << "Failed to replace gui root. Context is not initialized" << logwarn;
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
