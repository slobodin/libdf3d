#include <df3d/df3d.h>
#include "PlatformFile.h"

// FIXME: move somewhere AAssetMgr
#include <df3d/platform/android/JNIHelpers.h>

namespace df3d {

class PlatformFileAndroid : public PlatformFile
{
    AAsset *m_file;

public:
    PlatformFileAndroid(AAsset *file)
        : m_file(file)
    {

    }

    ~PlatformFileAndroid()
    {
        AAsset_close(m_file);
    }

    size_t getSize() override
    {
        return AAsset_getLength(m_file);
    }

    int32_t tell() override
    {
        return AAsset_getLength(m_file) - AAsset_getRemainingLength(m_file);
    }

    bool seek(int32_t offset, SeekDir origin) override
    {
        int whence;
        if (origin == SeekDir::CURRENT)
            whence = SEEK_CUR;
        else if (origin == SeekDir::BEGIN)
            whence = SEEK_SET;
        else if (origin == SeekDir::END)
            whence = SEEK_END;
        else
            return false;

        return AAsset_seek(m_file, offset, whence) != -1;
    }

    size_t read(void *buffer, size_t sizeInBytes) override
    {
        return AAsset_read(m_file, buffer, sizeInBytes);
    }
};

unique_ptr<PlatformFile> PlatformOpenFile(const char *path)
{
    auto mgr = AndroidServices::getAAssetManager();
    if (!mgr)
        return nullptr;

    AAsset *file = AAssetManager_open(mgr, path, AASSET_MODE_UNKNOWN);
    if (!file)
        return nullptr;

    return make_unique<PlatformFileAndroid>(file);
}

bool PlatformFileExists(const char *path)
{
    return PlatformOpenFile(path) != nullptr;
}

}
