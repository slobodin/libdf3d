#include "../AppDelegate.h"
#include <libdf3d/base/EngineController.h>
#include <libdf3d/platform/android/FileDataSourceAndroid.h>
#include <libdf3d/input/InputManager.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

struct AndroidTouch
{
    bool valid = false;
};

static const int MAX_TOUCHES = 16;

struct AndroidAppState
{
    JavaVM *javaVm = nullptr;

    unique_ptr<df3d::AppDelegate> appDelegate;
    unique_ptr<df3d::EngineController> engine;

    AndroidTouch touchesCache[MAX_TOUCHES];

    int primaryTouchId = -1;
};

static unique_ptr<AndroidAppState> g_appState;

namespace df3d {

void Application::setupDelegate(unique_ptr<AppDelegate> appDelegate)
{
    g_appState->appDelegate = std::move(appDelegate);
}

void Application::setTitle(const std::string &title)
{

}

EngineController& svc() { return *g_appState->engine; }

}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    if (vm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        df3d::glog << "Failed to get environment" << df3d::logcritical;
        return -1;
    }

    g_appState.reset(new AndroidAppState());
    g_appState->javaVm = vm;

    df3d::glog << "JNI_OnLoad success" << df3d::logmess;

    df3dInitialized();

    if (!g_appState->appDelegate)
    {
        df3d::glog << "Game code must set up application delegate in df3dInitialized" << df3d::logcritical;
        return -1;
    }

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_setAssetManager(JNIEnv* env, jobject cls, jobject assetManager)
{
    df3d::platform_impl::FileDataSourceAndroid::setAssetManager(AAssetManager_fromJava(env, assetManager));
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_init(JNIEnv* env, jclass cls, jint jscreenWidth, jint jscreenHeight)
{
    df3d::glog << "Doing native init" << df3d::logmess;

    if (!g_appState->engine)
    {
        g_appState->engine.reset(new df3d::EngineController());

        auto engineInitParams = g_appState->appDelegate->getInitParams();
        engineInitParams.windowWidth = jscreenWidth;
        engineInitParams.windowHeight = jscreenHeight;

        g_appState->engine->initialize(engineInitParams);
        if (!g_appState->appDelegate->onAppStarted())
            throw std::runtime_error("Game code initialization failed.");
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
    df3d::glog << "Activity was destroyed" << df3d::logmess;
    g_appState->appDelegate->onAppEnded();

    g_appState->engine->shutdown();
    g_appState.reset();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_draw(JNIEnv* env, jclass cls)
{
    g_appState->engine->step();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchDown(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (pointerId >= MAX_TOUCHES)
    {
        df3d::glog << "Touch limit exceeded" << df3d::logwarn;
        return;
    }

    /*
    auto &touch = g_appState->touchesCache[pointerId];
    touch.valid = true;
    touch.touch.x = x;
    touch.touch.y = y;
    touch.touch.dx = 0;
    touch.touch.dy = 0;
    touch.touch.id = pointerId;
    touch.touch.state = df3d::base::TouchEvent::State::DOWN;

    g_appState->appDelegate->onTouchEvent(touch.touch);
    */

    if (g_appState->primaryTouchId == -1)
    {
        df3d::svc().inputManager().setMousePosition(x, y);
        df3d::svc().inputManager().onMouseButtonPressed(df3d::MouseButton::LEFT);

        g_appState->primaryTouchId = pointerId;
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchUp(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (pointerId >= MAX_TOUCHES)
    {
        df3d::glog << "Touch limit exceeded" << df3d::logwarn;
        return;
    }

    /*
    auto &touch = g_appState->touchesCache[pointerId];
    touch.valid = false;
    touch.touch.x = x;
    touch.touch.y = y;
    touch.touch.dx = 0;
    touch.touch.dy = 0;
    touch.touch.id = pointerId;
    touch.touch.state = df3d::base::TouchEvent::State::UP;

    g_appState->appDelegate->onTouchEvent(touch.touch);
    */

    if (pointerId == g_appState->primaryTouchId)
    {
        df3d::svc().inputManager().setMousePosition(x, y);
        df3d::svc().inputManager().onMouseButtonReleased(df3d::MouseButton::LEFT);

        g_appState->primaryTouchId = -1;
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchMove(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (pointerId >= MAX_TOUCHES)
    {
        df3d::glog << "Touch limit exceeded" << df3d::logwarn;
        return;
    }

    /*
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
    */

    if (pointerId == g_appState->primaryTouchId)
        df3d::svc().inputManager().setMousePosition(x, y);
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchCancel(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (pointerId >= MAX_TOUCHES)
    {
        df3d::glog << "Touch limit exceeded" << df3d::logwarn;
        return;
    }

    /*
    auto &touch = g_appState->touchesCache[pointerId];
    touch.valid = false;
    touch.touch.x = x;
    touch.touch.y = y;
    touch.touch.dx = 0;
    touch.touch.dy = 0;
    touch.touch.id = pointerId;
    touch.touch.state = df3d::base::TouchEvent::State::CANCEL;

    g_appState->appDelegate->onTouchEvent(touch.touch);
    */

    if (pointerId == g_appState->primaryTouchId)
    {
        df3d::svc().inputManager().setMousePosition(x, y);
        df3d::svc().inputManager().onMouseButtonReleased(df3d::MouseButton::LEFT);

        g_appState->primaryTouchId = -1;
    }
}
