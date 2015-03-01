#include "df3d_pch.h"
#include "Application.h"

#if defined(DF3D_WINDOWS)
#include <platform/windows/WindowsApplication.h>
#elif defined(DF3D_WINDOWS_PHONE)
#include <platform/windows_phone/WindowsPhoneApplication.h>
#else
#error "Unsupported platform"
#endif

namespace df3d { namespace platform {

Application *Application::instance = nullptr;

Application::Application(const AppInitParams &params, AppDelegate *appDelegate)
    : m_appDelegate(appDelegate),
    m_appInitParams(params)
{
}

Application::~Application()
{

}

Application *Application::create(const AppInitParams &params, AppDelegate *appDelegate)
{
    assert(!Application::instance);

#if defined(DF3D_WINDOWS)
    Application::instance = new WindowsApplication(params, appDelegate);
#elif defined(DF3D_WINDOWS_PHONE)
    Application::instance = new WindowsPhoneApplication(params, appDelegate);
#else
    return nullptr;
#endif

    return Application::instance;
}

} }
