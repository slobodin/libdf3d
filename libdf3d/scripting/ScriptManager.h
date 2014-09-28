#pragma once

FWD_MODULE_CLASS(base, Controller)

namespace df3d { namespace scripting {

class DF3D_DLL ScriptManager : public boost::noncopyable
{
    friend class base::Controller;

    bool init() { return true; }
    void shutdown() { }

public:
    bool doFile(const char *fileName) { return false; }
    bool doString(const char *str) { return false; }
    void printError() { }
};

} }
