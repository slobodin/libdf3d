#include "JNIHelpers.h"

namespace df3d {

static void detachCurrentThread(void *a)
{
    AndroidServices::getJavaVM()->DetachCurrentThread();
}

JavaVM *AndroidServices::m_vm = nullptr;
pthread_key_t AndroidServices::m_envKey;
jobject AndroidServices::m_prefs;
jobject AndroidServices::m_localStorage;
jobject AndroidServices::m_services;
AAssetManager *AndroidServices::m_assetMgr = nullptr;

void AndroidServices::init(JavaVM *vm)
{
    m_vm = vm;
    pthread_key_create(&m_envKey, detachCurrentThread);

    DFLOG_DEBUG("AndroidServices::init success");
}

void AndroidServices::setServicesObj(jobject jservices)
{
    auto env = getEnv();

    jclass cls = env->GetObjectClass(jservices);
    jmethodID methId = env->GetMethodID(cls, "getLocalStorage", "()Ljava/lang/Object;");

    m_localStorage = env->CallObjectMethod(jservices, methId);
    m_localStorage = env->NewGlobalRef(m_localStorage);

    env->DeleteLocalRef(cls);

    m_services = env->NewGlobalRef(jservices);
}

void AndroidServices::setAAssetManager(AAssetManager *mgr)
{
    m_assetMgr = mgr;
}

JNIEnv* AndroidServices::getEnv()
{
    JNIEnv *env = (JNIEnv *)pthread_getspecific(m_envKey);
    if (env == nullptr)
    {
        jint ret = m_vm->GetEnv((void**)&env, JNI_VERSION_1_6);

        switch (ret)
        {
        case JNI_OK:
            pthread_setspecific(m_envKey, env);
            return env;
        case JNI_EDETACHED:
            if (m_vm->AttachCurrentThread(&env, nullptr) < 0)
            {
                DFLOG_WARN("Failed to AttachCurrentThread");
                return nullptr;
            }
            else
            {
                pthread_setspecific(m_envKey, env);
                return env;
            }
        default:
            DFLOG_WARN("Failed to AndroidServices::getEnv");
            return nullptr;
        }
    }

    return env;
}

jbyteArray AndroidServices::createByteArray(const uint8_t *data, size_t size)
{
    auto env = getEnv();

    jbyteArray result = env->NewByteArray(size);
    env->SetByteArrayRegion(result, 0, size, (const jbyte *)data);
    return result;
}

void AndroidServices::exitApp()
{
    auto env = getEnv();

    jclass cls = env->GetObjectClass(m_services);
    jmethodID methId = env->GetMethodID(cls, "exitApp", "()V");

    env->CallVoidMethod(m_services, methId);
}

std::string AndroidServices::jstringToStd(jstring jstr)
{
    auto env = getEnv();

    auto length = env->GetStringUTFLength(jstr);
    auto data = env->GetStringUTFChars(jstr, nullptr);

    std::string result(data, data + length);

    env->ReleaseStringUTFChars(jstr, data);

    return result;
}

size_t AndroidServices::getProcessMemUsage()
{
    auto env = getEnv();

    jclass cls = env->GetObjectClass(m_services);
    jmethodID methId = env->GetMethodID(cls, "getProcessMemUsage", "()J");

    jlong res = env->CallLongMethod(m_services, methId);

    env->DeleteLocalRef(cls);

    return res;
}

size_t AndroidServices::getGraphicsMemUsage()
{
    auto env = getEnv();

    jclass cls = env->GetObjectClass(m_services);
    jmethodID methId = env->GetMethodID(cls, "getGraphicsMemUsage", "()J");

    jlong res = env->CallLongMethod(m_services, methId);

    env->DeleteLocalRef(cls);

    return res;
}

}
