#pragma once

#if defined(DF3D_WINDOWS)
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#elif defined(__ANDROID__)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
elif defined(DF3D_WINDOWS_PHONE)
#error "Unsupported"
#else
#error "Unsupported platform"
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

