#pragma once

namespace df3d { namespace platform {

class CrashHandler
{
public:
    static void setup(const char *dumpFileName);
};

} }
