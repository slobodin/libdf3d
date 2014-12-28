#pragma once

namespace df3d { namespace scripting {

class DF3D_DLL ScriptManager : public boost::noncopyable
{
public:
    ScriptManager() = default;
    virtual ~ScriptManager() = default;

    virtual bool doFile(const char *fileName) = 0;
    virtual bool doString(const char *str) = 0;
    virtual void printError() = 0;
};

} }
