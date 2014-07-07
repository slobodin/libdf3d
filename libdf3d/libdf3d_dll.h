#pragma once

#if defined(WIN32)
#if defined(LIBDF3D_EXPORTS)
#define DF3D_DLL __declspec(dllexport)
#else
#define DF3D_DLL __declspec(dllimport)
#endif
#else
#define DF3D_DLL __attribute__((visibility("default")))
#endif
