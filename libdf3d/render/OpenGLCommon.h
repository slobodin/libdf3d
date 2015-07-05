#pragma once

#if defined(DF3D_WINDOWS)
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#elif defined(DF3D_ANDROID)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#elif defined(DF3D_WINDOWS_PHONE)
#include <angle/angle_gl.h>
#include <angle/EGL/egl.h>
#include <angle/EGL/eglext.h>
#elif defined(DF3D_LINUX)
#include <GL/glew.h>
#include <GL/gl.h>
#elif defined(DF3D_MACOSX)
#include <GL/glew.h>
#include <OpenGL/gl.h>
#elif defined(DF3D_IOS)
#include <UIKit/UIKit.h>
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
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
