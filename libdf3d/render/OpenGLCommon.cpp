#include "df3d_pch.h"
#include "OpenGLCommon.h"

namespace df3d { namespace render {

std::string checkGLError()
{
#if defined(__WIN32__)
    std::string errString;

    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR)
    {
        errString = (char *)gluErrorString(errCode);
    }

    return errString;
#else
    return "";
#endif
}

void checkAndPrintGLError(const char *file, int line)
{
#if defined(__WIN32__) && defined(_DEBUG)
    auto err = checkGLError();
    if (!err.empty())
        base::glog << "OpenGL error:" << err << ". File:" << file << ". Line:" << line << base::logwarn;
#endif
}

} }
