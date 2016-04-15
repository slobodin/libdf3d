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

void AndroidServices::init(JavaVM *vm)
{
    m_vm = vm;
    pthread_key_create(&m_envKey, detachCurrentThread);

    df3d::glog << "AndroidServices::init success" << df3d::logdebug;
}

void AndroidServices::setServicesObj(jobject jservices)
{
    auto env = getEnv();

    jclass cls = env->GetObjectClass(jservices);
    jmethodID methId = env->GetMethodID(cls, "getLocalStorage", "()Ljava/lang/Object;");

    m_localStorage = env->CallObjectMethod(jservices, methId);
    m_localStorage = env->NewGlobalRef(m_localStorage);

    env->DeleteLocalRef(cls);
}

JNIEnv* AndroidServices::getEnv()
{
    JNIEnv *env = (JNIEnv *)pthread_getspecific(m_envKey);
    if (env == nullptr)
    {
        jint ret = m_vm->GetEnv((void**)&env, JNI_VERSION_1_6);

        switch (ret)
        {
        case JNI_OK :
            pthread_setspecific(m_envKey, env);
            return env;
        case JNI_EDETACHED :
            if (m_vm->AttachCurrentThread(&env, nullptr) < 0)
            {
                glog << "Failed to AttachCurrentThread" << logwarn;

                return nullptr;
            }
            else
            {
                pthread_setspecific(m_envKey, env);
                return env;
            }
        default :
            glog << "Failed to AndroidServices::getEnv" << logwarn;
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

}
