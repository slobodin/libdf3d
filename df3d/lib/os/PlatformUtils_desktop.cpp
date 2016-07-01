#include "PlatformUtils.h"

#if defined(DF3D_WINDOWS)
#include <Windows.h>
#include <psapi.h>
#elif defined(DF3D_LINUX)
#include <X11/Xlib.h>
#else

#endif

namespace df3d {

size_t PlatformUtils::getProcessMemUsed()
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

size_t PlatformUtils::getProcessMemPeak()
{
#if defined(DF3D_WINDOWS)
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

    return pmc.PeakWorkingSetSize;
#else
    #warning Please implement
    return 0;
#endif
}

int PlatformUtils::getDPI()
{
#if defined(DF3D_WINDOWS)
    HDC hdc = GetDC(nullptr);
    int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    ReleaseDC(nullptr, hdc);

    return dpiX;
#elif defined(DF3D_LINUX)
    Display *dpy = XOpenDisplay(nullptr);

    double xres = ((double)DisplayWidth(dpy, 0) * 25.4) / ((double) DisplayWidthMM(dpy, 0));

    int res = (int)(xres + 0.5);

    XCloseDisplay(dpy);

    return res;
#else
#warning Please implement
    return 120;
#endif
}

}
