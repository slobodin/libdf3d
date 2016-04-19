#pragma once

#include <jni.h>
#include <pthread.h>

namespace df3d {

class AndroidServices
{
    static JavaVM *m_vm;
    static pthread_key_t m_envKey;

    static jobject m_prefs;
    static jobject m_localStorage;
    static jobject m_services;

public:
    static void init(JavaVM *vm);

    static void setServicesObj(jobject jservices);

    static jobject getPreferences() { return m_prefs; }
    static jobject getLocalStorage() { return m_localStorage; }

    static JNIEnv* getEnv();
    static JavaVM* getJavaVM() { return m_vm; }

    static jbyteArray createByteArray(const uint8_t *data, size_t size);

    static void exitApp();
};

}
