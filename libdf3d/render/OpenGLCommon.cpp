#include "df3d_pch.h"
#include "OpenGLCommon.h"

namespace df3d { namespace render {

std::string checkGLError()
{
#if defined(DF3D_WINDOWS)
    std::string errString;

    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR)
    {
        errString = (char *)gluErrorString(errCode);
    }

    return errString;
#elif defined(DF3D_WINDOWS_PHONE)
    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR)
        return "Unknown";

    return "";
#else
    return "";
#endif
}

void checkAndPrintGLError(const char *file, int line)
{
#if defined(DF3D_WINDOWS) || defined(DF3D_WINDOWS_PHONE) && defined(_DEBUG)
    auto err = checkGLError();
    if (!err.empty())
        base::glog << "OpenGL error:" << err << ". File:" << file << ". Line:" << line << base::logwarn;
#endif
}

} }
