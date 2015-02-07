#include "df3d_pch.h"
#include "LuaScriptManager.h"

#include <base/EngineController.h>
#include <resources/FileSystem.h>

namespace df3d { namespace scripting {

LuaScriptManager::LuaScriptManager()
{
    base::glog << "Initializing Lua" << base::logmess;

    m_luaState = luaL_newstate();
    if (!m_luaState)
        throw std::runtime_error("Failed to create lua state.");

    luaL_openlibs(m_luaState);
}

LuaScriptManager::~LuaScriptManager()
{
    lua_close(m_luaState);
}

bool LuaScriptManager::doFile(const char *fileName)
{
    auto fullp = g_fileSystem->fullPath(fileName);

    base::glog << "Executing" << fullp << base::logdebug;

    if (luaL_dofile(m_luaState, fullp.c_str()))
    {
        base::glog << "Lua script execution failed due to" << lua_tostring(m_luaState, -1) << base::logwarn;
        return false;
    }

    return true;
}

bool LuaScriptManager::doString(const char *str)
{
    return false;
}

void LuaScriptManager::printError()
{
    
}

} }