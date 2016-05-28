#include <libdf3d/platform/Platform.h>

#if defined(DF3D_WINDOWS)
#include <Windows.h>
#include <psapi.h>
#else

#endif

namespace df3d {

size_t Platform::getProcessMemoryUsed()
{
#if defined(DF3D_WINDOWS)
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

    return pmc.WorkingSetSize;
#else
#warning Please implement
    return 0;
#endif
}

int Platform::getDPI()
{
#if defined(DF3D_WINDOWS)
    HDC hdc = GetDC(nullptr);
    int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(nullptr, hdc);

    return dpiX;
#else
#warning Please implement
    return 120;
#endif
}

}
