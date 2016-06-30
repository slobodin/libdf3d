#pragma once

#include <squirrel.h>

namespace df3d {

class DF3D_DLL ScriptManager : utils::NonCopyable
{
    HSQUIRRELVM m_squirrel = nullptr;

    std::set<df3d::ResourceGUID> m_executedFiles;

public:
    ScriptManager();
    ~ScriptManager();

    void initialize();
    void shutdown();

    bool doFile(const std::string &fileName);
    bool doString(const SQChar *str);

    void gc();

    HSQUIRRELVM getVm() { return m_squirrel; }
};

}
