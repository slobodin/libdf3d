#pragma once

#if defined(__WINDOWS__)
#include <GL/gl3w.h>
#elif defined(__ANDROID__)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#endif

namespace df3d { namespace render {

//! Check OpenGL for errors.
/**
  * Returns error string or empty string if there are no errors.
  */
std::string checkGLError();
void checkAndPrintGLError(const char *file, int line);

#if defined(_DEBUG) || defined(DEBUG)
#define printOpenGLError() checkAndPrintGLError(__FILE__, __LINE__)
#else
#define printOpenGLError()
#endif

} }

