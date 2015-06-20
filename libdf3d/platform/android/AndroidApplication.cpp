#include "df3d_pch.h"
#include "AndroidApplication.h"

namespace df3d { namespace platform {

struct AndroidApplication::Impl
{

};

AndroidApplication::AndroidApplication(const AppInitParams &params, AppDelegate *appDelegate)
    : Application(params, appDelegate)
{
    // Init user code.
    if (!m_appDelegate->onAppStarted(params.windowWidth, params.windowHeight))
        throw std::runtime_error("Game code initialization failed.");
}

AndroidApplication::~AndroidApplication()
{

}

void AndroidApplication::run()
{
    using namespace std::chrono;

    TimePoint currtime, prevtime;
    currtime = prevtime = system_clock::now();

    m_appDelegate->onAppEnded();
}

void AndroidApplication::setTitle(const char *title)
{

}

} }
