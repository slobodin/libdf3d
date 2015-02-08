#include "df3d_pch.h"
#include "ScriptManager.h"

#include <lua/lua.hpp>

#include <base/EngineController.h>
#include <resources/FileSystem.h>
#include "lua/df3d_bindings.h"

namespace df3d { namespace scripting {

ScriptManager::ScriptManager()
{
    base::glog << "Initializing Lua" << base::logmess;

    m_luaState = luaL_newstate();
    if (!m_luaState)
        throw std::runtime_error("Failed to create lua state.");

    luaL_openlibs(m_luaState);

    bindGlm(m_luaState);
}

ScriptManager::~ScriptManager()
{
    lua_close(m_luaState);
}

bool ScriptManager::doFile(const char *fileName)
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

bool ScriptManager::doString(const char *str)
{
    return false;
}

void ScriptManager::printError()
{

}

} }
