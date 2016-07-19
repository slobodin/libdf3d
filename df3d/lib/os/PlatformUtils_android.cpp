#include "PlatformUtils.h"

#include <malloc.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/configuration.h>
// FIXME: move somewhere AAssetMgr
#include <df3d/platform/android/JNIHelpers.h>

namespace df3d {

size_t PlatformUtils::getProcessMemUsed()
{
    return AndroidServices::getProcessMemUsage();
}

size_t PlatformUtils::getProcessMemPeak()
{
    DFLOG_WARN("PlatformUtils::getProcessMemPeak is not implemented for Android");
    return 0;
}

int PlatformUtils::getDPI()
{
    AConfiguration *config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, AndroidServices::getAAssetManager());
    int32_t density = AConfiguration_getDensity(config);
    AConfiguration_delete(config);

    if (density == 0 || density == ACONFIGURATION_DENSITY_NONE)
        return 120;

    return density;
}

}
