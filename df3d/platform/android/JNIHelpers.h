#pragma once

#include <jni.h>
#include <pthread.h>
#include <android/asset_manager.h>

namespace df3d {

class AndroidServices
{
    static JavaVM *m_vm;
    static pthread_key_t m_envKey;

    static jobject m_prefs;
    static jobject m_localStorage;
    static jobject m_services;

    static AAssetManager *m_assetMgr;

public:
    static void init(JavaVM *vm);

    static void setServicesObj(jobject jservices);
    static void setAAssetManager(AAssetManager *mgr);

    static jobject getPreferences() { return m_prefs; }
    static jobject getLocalStorage() { return m_localStorage; }

    static JNIEnv* getEnv();
    static JavaVM* getJavaVM() { return m_vm; }
    static AAssetManager* getAAssetManager() { return m_assetMgr; }

    static jbyteArray createByteArray(const uint8_t *data, size_t size);

    static void exitApp();

    static std::string jstringToStd(jstring jstr);

    static size_t getProcessMemUsage();
    static size_t getGraphicsMemUsage();
};

}
