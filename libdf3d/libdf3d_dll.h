#pragma once

#if defined(DF3D_SHARED_LIBRARY)
    #if defined(DF3D_WINDOWS) || defined(DF3D_WINDOWS_PHONE)
        #if defined(LIBDF3D_EXPORTS)
            #define DF3D_DLL __declspec(dllexport)
        #else
            #define DF3D_DLL __declspec(dllimport)
        #endif
    #else
        #define DF3D_DLL __attribute__((visibility("default")))
    #endif
#else
    #define DF3D_DLL
#endif
