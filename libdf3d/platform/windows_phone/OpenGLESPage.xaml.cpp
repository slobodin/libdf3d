#include "pch.h"
#include "OpenGLESPage.xaml.h"

#include "OpenGLES.h"
#include <Ships3dAppDelegate.h>
#include <libdf3d_dll.h>
#include <base/Common.h>
#include <base/SystemsMacro.h>
#include <platform/Application.h>
#include <platform/AppDelegate.h>

using namespace df3d_winrt;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

OpenGLESPage::OpenGLESPage(df3d::platform::AppDelegate *appDelegate)
    : m_appDelegate(appDelegate),
    mRenderSurface(EGL_NO_SURFACE),
    mCustomRenderSurfaceSize(0, 0),
    mUseCustomRenderSurfaceSize(false)
{
    mOpenGLES = make_unique<OpenGLES>();

    InitializeComponent();

    Window::Current->CoreWindow->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &OpenGLESPage::OnVisibilityChanged);
    swapChainPanel->SizeChanged += ref new SizeChangedEventHandler(this, &OpenGLESPage::OnSwapChainPanelSizeChanged);
    this->Loaded += ref new RoutedEventHandler(this, &OpenGLESPage::OnPageLoaded);

    // Register our SwapChainPanel to get independent input pointer events
    auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^)
    {
        // The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread it's created on.
        m_coreInput = swapChainPanel->CreateCoreIndependentInputSource(CoreInputDeviceTypes::Mouse |CoreInputDeviceTypes::Touch |CoreInputDeviceTypes::Pen);
        // Register for pointer events, which will be raised on the background thread.
        m_coreInput->PointerPressed += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &OpenGLESPage::OnPointerPressed);
        m_coreInput->PointerMoved += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &OpenGLESPage::OnPointerMoved);
        m_coreInput->PointerReleased += ref new TypedEventHandler<Object^, PointerEventArgs^>(this, &OpenGLESPage::OnPointerReleased);

        // Begin processing input messages as they're delivered.
        m_coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
    });

    m_inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);

    mSwapChainPanelSize = { swapChainPanel->RenderSize.Width, swapChainPanel->RenderSize.Height };
}

OpenGLESPage::OpenGLESPage()
    : m_appDelegate(nullptr),
    m_appInitialized(false)
{

}

OpenGLESPage::~OpenGLESPage()
{
    StopRenderLoop();
    DestroyRenderSurface();
}

void OpenGLESPage::OnPageLoaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // The SwapChainPanel has been created and arranged in the page layout, so EGL can be initialized.
    CreateRenderSurface();
    StartRenderLoop();
}

void OpenGLESPage::OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args)
{
    if (args->Visible && mRenderSurface != EGL_NO_SURFACE)
    {
        StartRenderLoop();
    }
    else
    {
        StopRenderLoop();
    }
}

void OpenGLESPage::OnSwapChainPanelSizeChanged(Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
    // Size change events occur outside of the render thread.  A lock is required when updating
    // the swapchainpanel size
    critical_section::scoped_lock lock(mSwapChainPanelSizeCriticalSection);
    mSwapChainPanelSize = { e->NewSize.Width, e->NewSize.Height };
}

void OpenGLESPage::GetSwapChainPanelSize(int *width, int *height)
{
    critical_section::scoped_lock lock(mSwapChainPanelSizeCriticalSection);
    // If a custom render surface size is specified, return its size instead of
    // the swapchain panel size.
    if (mUseCustomRenderSurfaceSize)
    {
        *width = static_cast<int>(mCustomRenderSurfaceSize.Width);
        *height = static_cast<int>(mCustomRenderSurfaceSize.Height);
    }
    else
    {
        *width = static_cast<int>(mSwapChainPanelSize.Width);
        *height = static_cast<int>(mSwapChainPanelSize.Height);
    }
}

void OpenGLESPage::CreateRenderSurface()
{
    if (mOpenGLES)
    {
        //
        // A Custom render surface size can be specified by uncommenting the following lines.
        // The render surface will be automatically scaled to fit the entire window.  Using a
        // smaller sized render surface can result in a performance gain.
        //
        //mCustomRenderSurfaceSize = Size(340, 400);
        //mUseCustomRenderSurfaceSize = true;

        mRenderSurface = mOpenGLES->CreateSurface(swapChainPanel, mUseCustomRenderSurfaceSize ? &mCustomRenderSurfaceSize : nullptr);
    }
}

void OpenGLESPage::DestroyRenderSurface()
{
    if (mOpenGLES)
        mOpenGLES->DestroySurface(mRenderSurface);

    mRenderSurface = EGL_NO_SURFACE;
}

void OpenGLESPage::RecoverFromLostDevice()
{
    // Stop the render loop, reset OpenGLES, recreate the render surface
    // and start the render loop again to recover from a lost device.
    StopRenderLoop();

    {
        critical_section::scoped_lock lock(mRenderSurfaceCriticalSection);

        DestroyRenderSurface();
        mOpenGLES->Reset();
        CreateRenderSurface();
    }

    StartRenderLoop();
}

void OpenGLESPage::StartRenderLoop()
{
    using namespace std::chrono;

    // If the render loop is already running then do not start another thread.
    if (mRenderLoopWorker != nullptr && mRenderLoopWorker->Status == AsyncStatus::Started)
        return;

    // Create a task for rendering that will be run on a background thread.
    auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
    {
        critical_section::scoped_lock lock(mRenderSurfaceCriticalSection);

        mOpenGLES->MakeCurrent(mRenderSurface);

        if (!m_appInitialized)
        {
            GLsizei width, height;
            GetSwapChainPanelSize(&width, &height);
            m_appDelegate->onAppStarted(width, height);

            m_appInitialized = true;
        }

        TimePoint currtime, prevtime;
        currtime = prevtime = system_clock::now();

        while (action->Status == AsyncStatus::Started)
        {
            currtime = system_clock::now();

            ProcessPointers();
            m_appDelegate->onAppUpdate(df3d::IntervalBetween(currtime, prevtime));

            // The call to eglSwapBuffers might not be successful (i.e. due to Device Lost)
            // If the call fails, then we must reinitialize EGL and the GL resources.
            if (mOpenGLES->SwapBuffers(mRenderSurface) != GL_TRUE)
            {
                // XAML objects like the SwapChainPanel must only be manipulated on the UI thread.
                swapChainPanel->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([=]()
                {
                    RecoverFromLostDevice();
                }, CallbackContext::Any));

                return;
            }

            prevtime = currtime;
        }
    });

    // Run task on a dedicated high priority background thread.
    mRenderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void OpenGLESPage::StopRenderLoop()
{
    if (mRenderLoopWorker)
    {
        mRenderLoopWorker->Cancel();
        mRenderLoopWorker = nullptr;
    }
}

void OpenGLESPage::ProcessPointers()
{
    // FIXME:
    if (!m_appInitialized)
        return;

    std::shared_ptr<TouchEvent> touch;
    while (pointers.try_pop(touch))
    {
        switch (touch->type)
        {
        case TouchType::TOUCH_DOWN:
        {
            df3d::base::MouseButtonEvent ev;
            ev.x = touch->x;
            ev.y = touch->y;
            ev.state = df3d::base::MouseButtonEvent::State::PRESSED;
            ev.button = df3d::base::MouseButtonEvent::Button::LEFT;
            m_appDelegate->onMouseButtonEvent(ev);
        }
            break;
        case TouchType::TOUCH_MOVED:
        {
            df3d::base::MouseMotionEvent ev;
            ev.x = touch->x;
            ev.y = touch->y;
            ev.leftPressed = true;
            m_appDelegate->onMouseMotionEvent(ev);
        }
            break;
        case TouchType::TOUCH_UP:
        {
            df3d::base::MouseButtonEvent ev;
            ev.x = touch->x;
            ev.y = touch->y;
            ev.state = df3d::base::MouseButtonEvent::State::RELEASED;
            ev.button = df3d::base::MouseButtonEvent::Button::LEFT;
            m_appDelegate->onMouseButtonEvent(ev);
        }
            break;
        }
    }
}

void OpenGLESPage::OnPointerPressed(Object^ sender, PointerEventArgs^ e)
{
    pointers.push(std::make_shared<TouchEvent>(TouchType::TOUCH_DOWN, e));
}

void OpenGLESPage::OnPointerMoved(Object^ sender, PointerEventArgs^ e)
{
    pointers.push(std::make_shared<TouchEvent>(TouchType::TOUCH_MOVED, e));
}

void OpenGLESPage::OnPointerReleased(Object^ sender, PointerEventArgs^ e)
{
    pointers.push(std::make_shared<TouchEvent>(TouchType::TOUCH_UP, e));
}
