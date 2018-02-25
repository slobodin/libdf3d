#include <df3d/platform/android/JNIHelpers.h>
#include <df3d/platform/AppDelegate.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/input/InputManager.h>
#include <android/asset_manager_jni.h>

extern void df3d_JniOnLoad();

namespace df3d {

extern bool EngineInit(EngineInitParams params);
extern void EngineShutdown();

static bool g_haveJoystickAtStart = false;  // A hack

void Application::setTitle(const std::string &title)
{

}

void Application::quit()
{
    auto env = df3d::AndroidServices::getEnv();

    auto cls = env->FindClass("org/flaming0/df3d/Df3dActivity");
    auto methId = env->GetStaticMethodID(cls, "quitApp", "()V");

    env->CallStaticVoidMethod(cls, methId);
}

void Application::showCursor(bool show)
{

}

struct AndroidAppState
{
    bool initialized = false;
    JavaVM *javaVm = nullptr;
    AppDelegate *appDelegate = nullptr;
};

static AndroidAppState g_appState;

}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    df3d_JniOnLoad();

    JNIEnv *env;
    if (vm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        DFLOG_CRITICAL("Failed to get environment");
        return -1;
    }

    df3d::MemoryManager::init();

    df3d::g_appState.javaVm = vm;
    df3d::g_appState.appDelegate = df3d::AppDelegate::getInstance();
    DF3D_ASSERT(df3d::g_appState.appDelegate != nullptr);

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

extern "C" JNIEXPORT jboolean JNICALL Java_org_flaming0_df3d_NativeBindings_init(JNIEnv* env, jclass cls, jint jscreenWidth, jint jscreenHeight)
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
            return false;
        }

        DFLOG_MESS("Engine initialized.");

        if (df3d::g_haveJoystickAtStart)
            df3d::svc().inputManager().addController(0, df3d::MFI_CONTROLLER_GAMEPAD);

        // Init game code.
        if (!df3d::g_appState.appDelegate->onAppStarted())
        {
            DFLOG_CRITICAL("Game code initialization failed");
            return false;
        }

        DFLOG_MESS("Game code initialized.");

        df3d::g_appState.initialized = true;

        return true;
    }
    else
    {
        df3d::g_appState.appDelegate->onRenderRecreated();

        return true;
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onResume(JNIEnv* env, jclass cls)
{
    if (df3d::g_appState.initialized)
    {
        df3d::g_appState.appDelegate->onAppWillEnterForeground();
        df3d::g_appState.appDelegate->onAppDidBecomeActive();
    }

}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onPause(JNIEnv* env, jclass cls)
{
    if (df3d::g_appState.initialized) {
        df3d::g_appState.appDelegate->onAppWillResignActive();
        df3d::g_appState.appDelegate->onAppDidEnterBackground();
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onDestroy(JNIEnv* env, jclass cls)
{
    if (df3d::g_appState.initialized) {
        DFLOG_MESS("Activity is being destroyed");

        df3d::g_appState.appDelegate->onAppEnded();

        df3d::EngineShutdown();
        df3d::MemoryManager::shutdown();
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onRenderDestroyed(JNIEnv* env, jclass cls)
{
//    if (df3d::g_appState.appDelegate)
//        df3d::g_appState.appDelegate->onRenderDestroyed();
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_draw(JNIEnv* env, jclass cls)
{
    if (df3d::g_appState.initialized) {
        df3d::svc().step();
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchDown(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (df3d::g_appState.initialized) {
        df3d::svc().inputManager().onTouch(pointerId, x, y, df3d::Touch::State::DOWN);
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchUp(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{    if (df3d::g_appState.initialized) {
        df3d::svc().inputManager().onTouch(pointerId, x, y, df3d::Touch::State::UP);
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchMove(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (df3d::g_appState.initialized) {
        df3d::svc().inputManager().onTouch(pointerId, x, y, df3d::Touch::State::MOVING);
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_NativeBindings_onTouchCancel(JNIEnv* env, jclass cls, jint pointerId, jfloat x, jfloat y)
{
    if (df3d::g_appState.initialized) {
        df3d::svc().inputManager().onTouch(pointerId, x, y, df3d::Touch::State::CANCEL);
    }
}

// CONTROLLER SUPPORT

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerConnected(
        JNIEnv *env, jobject obj, jint deviceId)
{
    if (df3d::g_appState.initialized) {
        df3d::svc().inputManager().addController(deviceId, df3d::MFI_CONTROLLER_GAMEPAD);

        if (auto l = df3d::svc().inputManager().getMfiControllerListener())
            l->MFiControllerConnected();
    }
    else
        df3d::g_haveJoystickAtStart = true;
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerDisconnected(
        JNIEnv *env, jobject obj, jint deviceId)
{
    if (df3d::g_appState.initialized) {
        df3d::svc().inputManager().removeController(deviceId);

        if (auto l = df3d::svc().inputManager().getMfiControllerListener())
            l->MFiControllerDisconnected();
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedA(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener())
            l->Mfi_buttonA_Pressed(pressed);
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedX(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener())
            l->Mfi_buttonX_Pressed(pressed);
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedY(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener())
            l->Mfi_buttonY_Pressed(pressed);
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedB(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener())
            l->Mfi_buttonB_Pressed(pressed);
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedMenu(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            if (!pressed)
                l->Mfi_buttonMenu_Pressed();
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedDPadLeft(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            l->Mfi_DPadLeft_Pressed(pressed);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedDPadRight(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            l->Mfi_DPadRight_Pressed(pressed);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedDPadUp(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            l->Mfi_DPadUp_Pressed(pressed);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedDPadDown(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            l->Mfi_DPadDown_Pressed(pressed);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedR1(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            l->Mfi_RightShoulder_Changed(0.0f, pressed);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedR2(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            l->Mfi_RightTrigger_Changed(0.0f, pressed);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedL1(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            l->Mfi_LeftShoulder_Changed(0.0f, pressed);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerButtonPressedL2(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            l->Mfi_LeftTrigger_Changed(0.0f, pressed);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_GamePadHelper_nativeControllerThumbStickEvent(
        JNIEnv *env, jobject obj, jboolean isLeft, jfloat x, jfloat y)
{
    if (df3d::g_appState.initialized) {
        if (auto l = df3d::svc().inputManager().getMfiControllerListener()) {
            if (isLeft)
                l->Mfi_LeftThumbStick_Changed(x, y);
            else
                l->Mfi_RightThumbStick_Changed(x, y);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_df3d_Df3dActivity_nativeHardwareBackPressed(
        JNIEnv *env, jobject obj, jboolean pressed)
{
    if (df3d::g_appState.initialized && df3d::g_appState.appDelegate) {
        df3d::g_appState.appDelegate->onAndroidBackButtonPressed(pressed);
    }
}

