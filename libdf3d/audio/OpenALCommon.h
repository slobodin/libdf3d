#pragma once

#include <AL/al.h>
#include <AL/alc.h>

namespace df3d { namespace audio {

std::string checkALError();
void checkAndPrintALError(const char *file, int line);

#if defined(_DEBUG) || defined(DEBUG)
#define printOpenALError() audio::checkAndPrintALError(__FILE__, __LINE__)
#else
#define printOpenALError()
#endif

} }

