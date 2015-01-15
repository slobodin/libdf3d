#pragma once

#include <scripting/ScriptManager.h>
//#include "lua/lua.hpp"

namespace df3d { namespace scripting {

class LuaScriptManager : public ScriptManager
{
    //lua_State *m_luaState;

public:
    LuaScriptManager();
    ~LuaScriptManager();

    bool doFile(const char *fileName) override;
    bool doString(const char *str) override;
    void printError() override;
};

} }