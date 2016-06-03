#pragma once

#if defined(DF3D_IOS)
#include <al.h>
#include <alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

namespace df3d { namespace audio_impl {

std::string checkALError();
void checkAndPrintALError(const char *file, int line);

} }

#if defined(_DEBUG) || defined(DEBUG)
#define printOpenALError() audio_impl::checkAndPrintALError(__FILE__, __LINE__)
#else
#define printOpenALError()
#endif
