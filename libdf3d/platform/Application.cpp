#include "df3d_pch.h"
#include "Application.h"

#if defined(DF3D_WINDOWS)
#include <platform/windows/WindowsApplication.h>
#elif defined(DF3D_WINDOWS_PHONE)

#else
#error "Unsupported platform"
#endif

namespace df3d { namespace platform {

Application::Application(const AppInitParams &params, AppDelegate *appDelegate)
    : m_appDelegate(appDelegate),
    m_appInitParams(params)
{
}

Application::~Application()
{

}

unique_ptr<Application> Application::create(const AppInitParams &params, AppDelegate *appDelegate)
{
#if defined(DF3D_WINDOWS)
    return make_unique<WindowsApplication>(params, appDelegate);
#elif defined(DF3D_WINDOWS_PHONE)
    assert(false);
    return nullptr;
#else
    return nullptr;
#endif
}

} }