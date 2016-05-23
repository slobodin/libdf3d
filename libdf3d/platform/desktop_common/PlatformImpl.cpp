#include <libdf3d/platform/Platform.h>

#if defined(DF3D_WINDOWS)
#include <Windows.h>
#include <psapi.h>
#endif

namespace df3d {

size_t Platform::getProcessMemoryUsed()
{
#if defined(DF3D_WINDOWS)
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

    return pmc.WorkingSetSize;
#else
#error "Not implemented"
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
#error "Not implemented"
#endif
}

}
