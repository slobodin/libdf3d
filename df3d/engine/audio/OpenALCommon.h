#pragma once

#if defined(DF3D_IOS) || defined(DF3D_MACOSX)
#include <al.h>
#include <alc.h>
#else
#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#endif

namespace df3d {

std::string CheckALError();
void CheckAndPrintALError(const char *file, int line);

}

#if defined(_DEBUG) || defined(DEBUG)
#define printOpenALError() CheckAndPrintALError(__FILE__, __LINE__)
#else
#define printOpenALError()
#endif
