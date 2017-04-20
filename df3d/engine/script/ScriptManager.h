#pragma once

#include <squirrel.h>
#include <df3d/lib/NonCopyable.h>

namespace df3d {

class ScriptManager : NonCopyable
{
    HSQUIRRELVM m_squirrel = nullptr;

public:
    ScriptManager() = default;
    ~ScriptManager() = default;

    void initialize();
    void shutdown();

    bool doFile(const char *fileName);
    bool doString(const SQChar *str);

    void gc();

    HSQUIRRELVM getVm() { return m_squirrel; }
};

}
