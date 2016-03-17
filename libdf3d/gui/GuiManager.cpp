#include "GuiManager.h"

#include <tb_core.h>
#include <tb_debug.h>
#include <animation/tb_widget_animation.h>
#include "impl/TBInterface.h"
#include "impl/RocketInterface.h"
#include "impl/RocketKeyCodesAdapter.h"
#include <libdf3d/base/EngineController.h>
#include <libdf3d/input/InputEvents.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

namespace df3d {

GuiManager::GuiManager()
{

}

GuiManager::~GuiManager()
{

}

void GuiManager::initialize(int contextWidth, int contextHeight)
{
    glog << "Initializing libRocket" << logmess;

    m_width = contextWidth;
    m_height = contextHeight;

    using namespace Rocket;

    m_renderInterface.reset(new gui_impl::RenderInterface());
    m_systemInterface.reset(new gui_impl::SystemInterface());
    m_fileInterface.reset(new gui_impl::FileInterface());

    SetRenderInterface(m_renderInterface.get());
    SetSystemInterface(m_systemInterface.get());
    SetFileInterface(m_fileInterface.get());

    // Initialize core.
    if (!Core::Initialise())
        throw std::runtime_error("Can not initialize Rocket GUI library");

    // Initialize controls library.
    Controls::Initialise();

    // Create GUI context.
    m_rocketContext = Core::CreateContext("main", Core::Vector2i(contextWidth, contextHeight));

    // Initialize debugger.
#ifdef ENABLE_ROCKET_DEBUGGER
    if (!Rocket::Debugger::Initialise(m_rocketContext))
        glog << "Failed to initialize Rocket GUI debugger" << logwarn;

    // FIXME: setting new system interface as rocket debugger sets its own interface which
    // do not implement all methods.
    SetSystemInterface(m_systemInterface.get());
#endif


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

    m_rocketContext->RemoveReference();
    Rocket::Core::Shutdown();

    m_rocketContext = nullptr;
}

void GuiManager::update()
{
    tb::TBAnimationManager::Update();
    m_root->InvokeProcessStates();
    m_root->InvokeProcess();
}

void GuiManager::replaceRoot()
{
    m_root = make_unique<tb::TBWidget>();
    m_root->SetRect(tb::TBRect(0, 0, m_width, m_height));
}

RocketDocument GuiManager::loadDocument(const std::string &name)
{
    auto doc = m_rocketContext->LoadDocument(name.c_str());
    if (!doc)
        return nullptr;

    doc->RemoveReference();
    return doc;
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
