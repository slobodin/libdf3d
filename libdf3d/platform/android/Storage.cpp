#include <libdf3d/io/Storage.h>

#include "JNIHelpers.h"
#include <libdf3d/utils/JsonUtils.h>

namespace df3d { namespace platform_impl {

class AndroidStorage : public Storage
{
    void saveToFileSystem(const uint8_t *data, size_t size) override
    {
        auto env = AndroidServices::getEnv();
        auto prefsObj = AndroidServices::getLocalStorage();

        jclass cls = env->GetObjectClass(prefsObj);
        jmethodID methId = env->GetMethodID(cls, "writeToInternalFile", "(Ljava/lang/String;[B)Z");

        jbyteArray jdata = AndroidServices::createByteArray(data, size);
        jstring jfilename = env->NewStringUTF("df3d");

        bool saveResult = env->CallBooleanMethod(prefsObj, methId, jfilename, jdata);
        if (!saveResult)
            glog << "AndroidStorage::saveToFileSystem failed" << df3d::logwarn;
        else
            df3d::glog << "AndroidStorage::saveToFileSystem success" << df3d::logdebug;

        env->DeleteLocalRef(jdata);
        env->DeleteLocalRef(jfilename);
        env->DeleteLocalRef(cls);
    }

    bool getFromFileSystem(uint8_t **data, size_t *size) override
    {
        *size = 0;

        auto env = AndroidServices::getEnv();
        auto prefsObj = AndroidServices::getLocalStorage();

        jclass cls = env->GetObjectClass(prefsObj);
        jmethodID methId = env->GetMethodID(cls, "readInternalFile", "(Ljava/lang/String;)[B");
        jstring jfilename = env->NewStringUTF("df3d");

        jbyteArray jdata = (jbyteArray) env->CallObjectMethod(prefsObj, methId, jfilename);
        if (jdata != nullptr)
        {
            *size = env->GetArrayLength(jdata);
            *data = new uint8_t[*size];
            env->GetByteArrayRegion(jdata, 0, *size, (jbyte*)*data);
        }

        env->DeleteLocalRef(jdata);
        env->DeleteLocalRef(jfilename);
        env->DeleteLocalRef(cls);

        df3d::glog << "AndroidStorage::getFromFileSystem got" << *size << "of data" << df3d::logdebug;

        return size != 0;
    }

public:
    AndroidStorage(const std::string &filename)
        : Storage(filename)
    {

    }
};

}

Storage* Storage::create(const std::string &filename)
{
    return new platform_impl::AndroidStorage(filename);
}

}
