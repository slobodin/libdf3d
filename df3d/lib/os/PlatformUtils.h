#pragma once

namespace df3d {

class PlatformUtils
{
public:
    static size_t getProcessMemUsed();
    static size_t getProcessMemPeak();
    static int getDPI();
};

}
