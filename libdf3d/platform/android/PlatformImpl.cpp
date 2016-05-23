#include <libdf3d/platform/Platform.h>

#include <malloc.h>
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/configuration.h>
#include "FileDataSourceAndroid.h"

namespace df3d {

size_t Platform::getProcessMemoryUsed()
{
    return mallinfo().uordblks;
}

int Platform::getDPI()
{
    auto aassetMgr = platform_impl::FileDataSourceAndroid::getAAssetManager();

    AConfiguration *config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, aassetMgr);
    int32_t density = AConfiguration_getDensity(config);
    AConfiguration_delete(config);

    if (density == 0 || density == ACONFIGURATION_DENSITY_NONE)
        return 120;

    return density;
}

}
