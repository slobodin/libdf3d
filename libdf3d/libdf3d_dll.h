#pragma once

// Force static for now
#if 0
#if defined(DF3D_WINDOWS) || defined(DF3D_WINDOWS_PHONE)
#if defined(LIBDF3D_EXPORTS)
#define DF3D_DLL __declspec(dllexport)
#else
#define DF3D_DLL __declspec(dllimport)
#endif
#elif defined(DF3D_ANDROID)
#define DF3D_DLL __attribute__((visibility("default")))
#else
#define DF3D_DLL
#endif
#endif

#define DF3D_DLL
