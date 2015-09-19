#include "OpenALCommon.h"

namespace df3d { namespace audio {

std::string checkALError()
{
#if defined(DF3D_DESKTOP)
    std::string errString;

    ALenum errCode = alGetError();
    if (errCode != AL_NO_ERROR)
    {
        errString = (char *)alGetString(errCode);
    }

    return errString;
#else
    return "";
#endif
}

void checkAndPrintALError(const char *file, int line)
{
#ifdef _DEBUG
    auto err = checkALError();
    if (!err.empty())
        base::glog << "OpenAL error:" << err << ". File:" << file << ". Line:" << line << base::logwarn;
#endif
}

} }
