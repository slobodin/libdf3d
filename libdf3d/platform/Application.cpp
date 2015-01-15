#include "df3d_pch.h"
#include "Application.h"

#if defined(DF3D_WINDOWS)
#include <platform/windows/WindowsApplication.h>
#else
#error "Unsupported platform"
#endif

namespace df3d { namespace platform {

Application::Application()
{
}

Application::~Application()
{

}

Application *Application::create(const AppInitParams &params)
{
#if defined(DF3D_WINDOWS)
    return new WindowsApplication(params);
#else
    return nullptr;
#endif
}

} }