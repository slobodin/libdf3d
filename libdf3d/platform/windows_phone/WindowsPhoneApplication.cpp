#include "df3d_pch.h"
#include "WindowsPhoneApplication.h"

#include "OpenGLESBridge.h"

#include <wrl.h>
#include <wrl/client.h>
#include <ppltasks.h>

namespace df3d { namespace platform {

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
    friend ref class ApplicationSource;

    bool m_windowClosed = false;
    bool m_windowVisible = true;

    std::unique_ptr<df3d::platform::OpenGLESBridge> m_openglesBridge;
    AppDelegate *m_appDelegate;

    App(AppDelegate *appDelegate)
        : m_appDelegate(appDelegate)
    {

    }

public:
    // IFrameworkView Methods.
    virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView)
    {
        // Register event handlers for app lifecycle. This example includes Activated, so that we
        // can make the CoreWindow active and start rendering on the window.
        applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);
        CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);
        CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &App::OnResuming);

        // At this point we have access to the device. 
        // We can create the device-dependent resources.
        m_openglesBridge = std::make_unique<df3d::platform::OpenGLESBridge>();
    }

    virtual void SetWindow(Windows::UI::Core::CoreWindow^ window)
    {
        static bool CRUTCH = false;

        if (CRUTCH)
            throw Platform::Exception::CreateException(E_FAIL, L"CRUTCH");

        window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);
        window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

        DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

        DisplayInformation::DisplayContentsInvalidated += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
        window->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

        currentDisplayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

        currentDisplayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

        // Disable all pointer visual feedback for better performance when touching.
        // This is not supported on Windows Phone applications.
        auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
        pointerVisualizationSettings->IsContactFeedbackEnabled = false;
        pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;
#endif

        m_openglesBridge->initEGL(window);
        m_openglesBridge->updateWindowSize(Size(window->Bounds.Width, window->Bounds.Height));

        m_appDelegate->onAppStarted((int)window->Bounds.Width, (int)window->Bounds.Height);

        CRUTCH = true;
    }

    virtual void Load(Platform::String^ entryPoint)
    {

    }

    virtual void Run()
    {
        using namespace std::chrono;

        TimePoint currtime = system_clock::now();
        TimePoint prevtime = system_clock::now();

        while (!m_windowClosed)
        {
            if (m_windowVisible)
            {
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

                currtime = system_clock::now();

                m_appDelegate->onAppUpdate(IntervalBetween(currtime, prevtime));

                m_openglesBridge->swapBuffers();

                prevtime = currtime;

                // The call to eglSwapBuffers might not be successful (e.g. due to Device Lost)
                // If the call fails, then we must reinitialize EGL and the GL resources.
                //if (eglSwapBuffers(mEglDisplay, mEglSurface) != GL_TRUE)
                //{
                //    mTriangleRenderer.reset(nullptr);
                //    CleanupEGL();

                //    InitializeEGL(CoreWindow::GetForCurrentThread());
                //    RecreateRenderer();
                //}
            }
            else
            {
                CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
            }
        }

        //m_appDelegate->onAppEnded();
        m_openglesBridge.reset();
    }

    virtual void Uninitialize()
    {

    }

protected:
    // Application lifecycle event handlers.
    void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args)
    {
        // Run() won't start until the CoreWindow is activated.
        CoreWindow::GetForCurrentThread()->Activate();
    }

    void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args)
    {
        // Save app state asynchronously after requesting a deferral. Holding a deferral
        // indicates that the application is busy performing suspending operations. Be
        // aware that a deferral may not be held indefinitely. After about five seconds,
        // the app will be forced to exit.
        SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

        create_task([this, deferral]()
        {
            // Insert your code here.

            deferral->Complete();
        });
    }

    void OnResuming(Platform::Object^ sender, Platform::Object^ args)
    {
        // Restore any data or state that was unloaded on suspend. By default, data
        // and state are persisted when resuming from suspend. Note that this event
        // does not occur if the app was previously terminated.

        // Insert your code here.
    }

    // Window event handlers.
#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
    void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args)
    {

    }
#endif

    void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args)
    {
        m_windowVisible = args->Visible;
    }

    void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args)
    {
        m_windowClosed = true;
    }

    // DisplayInformation event handlers.
#if !(WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
    void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)
    {

    }

    void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)
    {

    }

#endif
    void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)
    {

    }
};

ref class ApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
    friend class WindowsPhoneApplication;

    AppDelegate *m_appDelegate;

    ApplicationSource(AppDelegate *appDelegate)
        : m_appDelegate(appDelegate)
    {

    }

public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
    {
        return ref new App(m_appDelegate);
    }
};

WindowsPhoneApplication::WindowsPhoneApplication(const AppInitParams &params, AppDelegate *appDelegate)
    : Application(params, appDelegate)
{

}

WindowsPhoneApplication::~WindowsPhoneApplication()
{

}

void WindowsPhoneApplication::run()
{
    auto appSource = ref new ApplicationSource(m_appDelegate);
    CoreApplication::Run(appSource);
}

} }