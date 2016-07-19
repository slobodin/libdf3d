#include <df3d/platform/android/JNIHelpers.h>
#include <df3d/platform/AppDelegate.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/input/InputManager.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace df3d {

extern bool EngineInit(EngineInitParams params);
extern void EngineShutdown();

void Application::setTitle(const std::string &title)
{

}

struct AndroidAppState
{
    bool initialized = false;
    JavaVM *javaVm = nullptr;
    AppDelegate *appDelegate = nullptr;

    TouchID primaryTouchId = Touch::INVALID_ID;
};

static AndroidAppState g_appState;

}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    if (vm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        DFLOG_CRITICAL("Failed to get environment");
        return -1;
    }

    df3d::MemoryManager::init();

    df3d::g_appState.javaVm = vm;
    df3d::g_appState.appDelegate = df3d_GetAppDelegate();

    df3d::AndroidServices::init(vm);

    DFLOG_MESS("JNI_OnLoad success");

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_setAssetManager(JNIEnv* env, jobject cls, jobject assetManager)
{
    df3d::AndroidServices::setAAssetManager(AAssetManager_fromJava(env, assetManager));
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_servicesInitialized(JNIEnv* env, jobject cls, jobject services)
{
    df3d::AndroidServices::setServicesObj(services);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_init(JNIEnv* env, jclass cls, jint jscreenWidth, jint jscreenHeight)
{
    DFLOG_MESS("Doing native init...");

    if (!df3d::g_appState.initialized)
    {
        auto params = df3d::g_appState.appDelegate->getInitParams();
        params.windowWidth = jscreenWidth;
        params.windowHeight = jscreenHeight;

        // Init the engine.
        if (!df3d::EngineInit(params))
        {
            DFLOG_CRITICAL("Failed to init df3d");
            return;
        }

        // Init game code.
        if (!df3d::g_appState.appDelegate->onAppStarted())
        {
            DFLOG_CRITICAL("Game code initialization failed");
            return;
        }
    }
    else
    {
        df3d::g_appState.appDelegate->onRenderRecreated();
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onResume(JNIEnv* env, jclass cls)
{
    df3d::g_appState.appDelegate->onAppWillEnterForeground();
    df3d::g_appState.appDelegate->onAppDidBecomeActive();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onPause(JNIEnv* env, jclass cls)
{
    df3d::g_appState.appDelegate->onAppWillResignActive();
    df3d::g_appState.appDelegate->onAppDidEnterBackground();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onDestroy(JNIEnv* env, jclass cls)
{
    DFLOG_MESS("Activity is being destroyed");

    df3d::g_appState.appDelegate->onAppEnded();

    df3d::EngineShutdown();
    df3d::MemoryManager::shutdown();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onSurfaceDestroyed(JNIEnv* env, jclass cls)
{
    if (df3d::g_appState.appDelegate)
        df3d::g_appState.appDelegate->onRenderDestroyed();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_draw(JNIEnv* env, jclass cls)
{
    df3d::svc().step();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchDown(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    df3d::svc().inputManager().onTouch(pointerId, x, y, df3d::Touch::State::DOWN);

    if (df3d::g_appState.primaryTouchId == df3d::Touch::INVALID_ID)
    {
        // DFLOG_DEBUG("Primary touch down %d, %d", (int)x, (int)y);

        df3d::svc().inputManager().onMouseButtonPressed(df3d::MouseButton::LEFT, x, y);

        df3d::g_appState.primaryTouchId = pointerId;
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchUp(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    df3d::svc().inputManager().onTouch(pointerId, x, y, df3d::Touch::State::UP);

    if (pointerId == df3d::g_appState.primaryTouchId)
    {
        // DFLOG_DEBUG("Primary touch up %d, %d", (int)x, (int)y);

        df3d::svc().inputManager().onMouseButtonReleased(df3d::MouseButton::LEFT, x, y);

        df3d::g_appState.primaryTouchId = df3d::Touch::INVALID_ID;
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchMove(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    df3d::svc().inputManager().onTouch(pointerId, x, y, df3d::Touch::State::MOVING);

    if (pointerId == df3d::g_appState.primaryTouchId)
    {
        //df3d::glog << "Touch MOVE" << x << y << pointerId << df3d::logcritical;

        df3d::svc().inputManager().setMousePosition(x, y);
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchCancel(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    df3d::svc().inputManager().onTouch(pointerId, x, y, df3d::Touch::State::CANCEL);

    if (pointerId == df3d::g_appState.primaryTouchId)
    {
        //df3d::glog << "Touch CANCEL" << x << y << pointerId << df3d::logcritical;

        df3d::svc().inputManager().onMouseButtonReleased(df3d::MouseButton::LEFT, x, y);

        df3d::g_appState.primaryTouchId = df3d::Touch::INVALID_ID;
    }
}
