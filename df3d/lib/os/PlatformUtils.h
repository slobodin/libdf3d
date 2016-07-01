#pragma once

namespace df3d {

class DF3D_DLL PlatformUtils
{
public:
    static size_t getProcessMemUsed();
    static size_t getProcessMemPeak();
    static int getDPI();
};

}
