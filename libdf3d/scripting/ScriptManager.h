#pragma once

struct lua_State;

namespace df3d { namespace scripting {

class DF3D_DLL ScriptManager : public boost::noncopyable
{
    lua_State *m_luaState;

public:
    ScriptManager();
    ~ScriptManager();

    bool doFile(const char *fileName);
    bool doString(const char *str);
    void printError();

    void performBinding(std::function<void(lua_State *)> binder);
};

} }
