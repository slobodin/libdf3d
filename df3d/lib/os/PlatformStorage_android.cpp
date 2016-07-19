#include "PlatformStorage.h"

// FIXME: move somewhere AAssetMgr
#include <df3d/platform/android/JNIHelpers.h>

namespace df3d {

bool PlatformStorage::saveData(const char *id, const PodArray<uint8_t> &data)
{
    auto env = AndroidServices::getEnv();
    auto prefsObj = AndroidServices::getLocalStorage();

    jclass cls = env->GetObjectClass(prefsObj);
    jmethodID methId = env->GetMethodID(cls, "writeToInternalFile", "(Ljava/lang/String;[B)Z");

    jbyteArray jdata = AndroidServices::createByteArray(data.data(), data.size());
    jstring jfilename = env->NewStringUTF(id);

    bool saveResult = env->CallBooleanMethod(prefsObj, methId, jfilename, jdata);
    if (!saveResult)
        DFLOG_WARN("PlatformStorage::saveData failed");
    else
        DFLOG_MESS("PlatformStorage::saveData success");

    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(jfilename);
    env->DeleteLocalRef(cls);

    return saveResult;
}

void PlatformStorage::getData(const char *id, PodArray<uint8_t> &data)
{
    data.clear();

    auto env = AndroidServices::getEnv();
    auto prefsObj = AndroidServices::getLocalStorage();

    jclass cls = env->GetObjectClass(prefsObj);
    jmethodID methId = env->GetMethodID(cls, "readInternalFile", "(Ljava/lang/String;)[B");
    jstring jfilename = env->NewStringUTF(id);

    jbyteArray jdata = (jbyteArray) env->CallObjectMethod(prefsObj, methId, jfilename);
    if (jdata != nullptr)
    {
        auto size = env->GetArrayLength(jdata);

        data.resize(size);

        env->GetByteArrayRegion(jdata, 0, size, (jbyte*)&data[0]);
    }

    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(jfilename);
    env->DeleteLocalRef(cls);

    DFLOG_DEBUG("PlatformStorage::getData got %d of data", data.size());
}

}
