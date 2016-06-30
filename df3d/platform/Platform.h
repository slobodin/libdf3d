#pragma once

namespace df3d {

class DF3D_DLL Platform
{
public:
    static size_t getProcessMemUsed();
    static size_t getProcessMemPeak();
    static int getDPI();
};

}
