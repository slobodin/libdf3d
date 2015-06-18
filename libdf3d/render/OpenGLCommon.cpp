#include "df3d_pch.h"
#include "OpenGLCommon.h"

namespace df3d { namespace render {

std::string checkGLError()
{
#if defined(DF3D_WINDOWS) || defined(DF3D_LINUX)
    std::string errString;

    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR)
    {
        errString = (char *)gluErrorString(errCode);
    }

    return errString;
#elif defined(DF3D_WINDOWS_PHONE)
    GLenum errCode = glGetError();
    if (errCode == GL_NO_ERROR)
        return "";

    switch (errCode)
    {
    case GL_INVALID_ENUM:
        return "Invalid enum";
    case GL_INVALID_VALUE:
        return "Invalid value";
    case GL_INVALID_OPERATION:
        return "Invalid operation";
    case GL_OUT_OF_MEMORY:
        return "Out of memory";
    default:
        return "Unknown";
    }
#else
    return "";
#endif
}

void checkAndPrintGLError(const char *file, int line)
{
#ifdef _DEBUG
    auto err = checkGLError();
    if (!err.empty())
        base::glog << "OpenGL error:" << err << ". File:" << file << ". Line:" << line << base::logwarn;
#endif
}

} }
