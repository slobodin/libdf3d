#include "df3d_pch.h"
#include "LinuxApplication.h"

namespace df3d { namespace platform {

struct LinuxApplication::Impl
{

};

LinuxApplication::LinuxApplication(const AppInitParams &params, AppDelegate *appDelegate)
    : Application(params, appDelegate),
    m_pImpl(make_unique<Impl>())
{

    if (!m_appDelegate->onAppStarted(params.windowWidth, params.windowHeight))
        throw std::runtime_error("Game code initialization failed.");
}

LinuxApplication::~LinuxApplication()
{

}

void LinuxApplication::run()
{
    using namespace std::chrono;
//    MSG msg;

    TimePoint currtime, prevtime;
    currtime = prevtime = system_clock::now();

//    while (!m_pImpl->window->quitRequested())
//    {
//        // FIXME:
//        // Maybe queue messages?
//        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//        else
//        {
//            currtime = system_clock::now();

//            m_appDelegate->onAppUpdate(IntervalBetween(currtime, prevtime));

//            m_pImpl->window->swapBuffers();

//            prevtime = currtime;
//        }
//    }

    m_appDelegate->onAppEnded();

    m_pImpl.reset();
}

void LinuxApplication::setTitle(const char *title)
{

}

} }
