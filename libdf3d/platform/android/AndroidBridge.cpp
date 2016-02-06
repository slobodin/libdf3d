#include "../AppDelegate.h"
#include <libdf3d/platform/android/FileDataSourceAndroid.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

struct AndroidTouch
{
    df3d::base::TouchEvent touch;
    bool valid = false;
};

static const int MAX_TOUCHES = 16;

struct AndroidAppState
{
    JavaVM *javaVm = nullptr;
    df3d::platform::AppDelegate *appDelegate = nullptr;

    AndroidTouch touchesCache[MAX_TOUCHES];

    ~AndroidAppState()
    {
        SAFE_DELETE(appDelegate);
    }
};

AndroidAppState *g_appState = nullptr;

extern void df3dInitialized();

namespace df3d { namespace platform {

void setupDelegate(df3d::platform::AppDelegate *appDelegate)
{
    assert(g_appState);

    g_appState->appDelegate = appDelegate;

    df3d::base::glog << "App delegate was set up" << df3d::base::logmess;
}

} }

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    if (vm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        df3d::base::glog << "Failed to get environment" << df3d::base::logcritical;
        return -1;
    }

    g_appState = new AndroidAppState();
    g_appState->javaVm = vm;

    df3d::base::glog << "JNI_OnLoad success" << df3d::base::logmess;

    df3dInitialized();

    assert(g_appState->appDelegate && "Game code must set up application delegate");

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_setAssetManager(JNIEnv* env, jobject cls, jobject assetManager)
{
    df3d::platform::FileDataSourceAndroid::setAssetManager(AAssetManager_fromJava(env, assetManager));
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_init(JNIEnv* env, jclass cls, jint jscreenWidth, jint jscreenHeight)
{
    df3d::base::glog << "Doing native init" << df3d::base::logmess;

    // Must be initialized by client code!
    if (!g_engineController->initialized())
    {
        if (!g_appState->appDelegate->onAppStarted(jscreenWidth, jscreenHeight))
            df3d::base::glog << "Failed to initialize game code." << df3d::base::logcritical;

        // TODO:
        // Shutdown on failure.
    }
    else
    {
        // TODO:
        // Recreate GL resources.
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onResume(JNIEnv* env, jclass cls)
{
    g_appState->appDelegate->onAppResumed();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onPause(JNIEnv* env, jclass cls)
{
    g_appState->appDelegate->onAppPaused();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onDestroy(JNIEnv* env, jclass cls)
{
    df3d::base::glog << "Activity was destroyed" << df3d::base::logmess;
    g_appState->appDelegate->onAppEnded();

    SAFE_DELETE(g_appState);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_draw(JNIEnv* env, jclass cls, jdouble dt)
{
    g_appState->appDelegate->onAppUpdate((float)dt);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchDown(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (pointerId >= MAX_TOUCHES)
    {
        df3d::base::glog << "Touch limit exceeded" << df3d::base::logwarn;
        return;
    }

    auto &touch = g_appState->touchesCache[pointerId];
    touch.valid = true;
    touch.touch.x = x;
    touch.touch.y = y;
    touch.touch.dx = 0;
    touch.touch.dy = 0;
    touch.touch.id = pointerId;
    touch.touch.state = df3d::base::TouchEvent::State::DOWN;

    g_appState->appDelegate->onTouchEvent(touch.touch);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchUp(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (pointerId >= MAX_TOUCHES)
    {
        df3d::base::glog << "Touch limit exceeded" << df3d::base::logwarn;
        return;
    }

    auto &touch = g_appState->touchesCache[pointerId];
    touch.valid = false;
    touch.touch.x = x;
    touch.touch.y = y;
    touch.touch.dx = 0;
    touch.touch.dy = 0;
    touch.touch.id = pointerId;
    touch.touch.state = df3d::base::TouchEvent::State::UP;

    g_appState->appDelegate->onTouchEvent(touch.touch);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchMove(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (pointerId >= MAX_TOUCHES)
    {
        df3d::base::glog << "Touch limit exceeded" << df3d::base::logwarn;
        return;
    }

    auto &touch = g_appState->touchesCache[pointerId];
    if (touch.valid)
    {
        touch.touch.dx = x - touch.touch.x;
        touch.touch.dy = y - touch.touch.y;
    }

    touch.touch.x = x;
    touch.touch.y = y;
    touch.touch.id = pointerId;
    touch.touch.state = df3d::base::TouchEvent::State::MOVING;

    g_appState->appDelegate->onTouchEvent(touch.touch);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchCancel(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (pointerId >= MAX_TOUCHES)
    {
        df3d::base::glog << "Touch limit exceeded" << df3d::base::logwarn;
        return;
    }

    auto &touch = g_appState->touchesCache[pointerId];
    touch.valid = false;
    touch.touch.x = x;
    touch.touch.y = y;
    touch.touch.dx = 0;
    touch.touch.dy = 0;
    touch.touch.id = pointerId;
    touch.touch.state = df3d::base::TouchEvent::State::CANCEL;

    g_appState->appDelegate->onTouchEvent(touch.touch);
}
