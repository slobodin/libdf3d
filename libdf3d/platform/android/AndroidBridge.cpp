#include "df3d_pch.h"

#include "../AppDelegate.h"
#include <jni.h>

JavaVM *g_JavaVm = nullptr;
df3d::platform::AppDelegate *g_appDelegate = nullptr;

extern void df3dInitialized();

namespace df3d { namespace platform {

void setupDelegate(df3d::platform::AppDelegate *appDelegate)
{
    g_appDelegate = appDelegate;

    df3d::base::glog << "App delegate was set up" << df3d::base::logmess;
}

void run()
{

}

} }

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    g_JavaVm = vm;

    JNIEnv *env;
    if (vm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        df3d::base::glog << "Failed to get environment" << df3d::base::logcritical;
        return -1;
    }

    df3d::base::glog << "JNI_OnLoad success" << df3d::base::logmess;

    df3dInitialized();

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_si_NativeBindings_init(JNIEnv* env, jclass cls, jint jscreenWidth, jint jscreenHeight)
{
    /*
    if (!AppDelegate::instance())
    {
        auto delegate = df3dCreateAppDelegate();
        if (!delegate->onAppStarted(jscreenWidth, jscreenHeight))
            throw std::runtime_error("Game code initialization failed.");
    }
    */
}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_si_NativeBindings_onResume(JNIEnv* env, jclass cls)
{

}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_si_NativeBindings_onPause(JNIEnv* env, jclass cls)
{

}

extern "C" JNIEXPORT void JNICALL Java_org_flaming0_si_NativeBindings_onDestroy(JNIEnv* env, jclass cls)
{
    
}
