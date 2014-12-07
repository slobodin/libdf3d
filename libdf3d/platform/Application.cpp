#include "df3d_pch.h"
#include "Application.h"

#if defined(__WINDOWS__)
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
#if defined(__WINDOWS__)
    return new WindowsApplication(params);
#else
    return nullptr;
#endif
}

} }