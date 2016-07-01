#include "OpenALCommon.h"

namespace df3d {

std::string CheckALError()
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

void CheckAndPrintALError(const char *file, int line)
{
#ifdef _DEBUG
    auto err = CheckALError();
    if (!err.empty())
        DFLOG_WARN("OpenAL error: %s. File: %s, line: %d", err.c_str(), file, line);
#endif
}

}
