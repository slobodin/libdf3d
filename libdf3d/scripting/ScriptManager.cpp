#include "df3d_pch.h"
#include "ScriptManager.h"

#include <lua/lua.hpp>
#include <selene/selene.h>

#include <base/SystemsMacro.h>

namespace df3d { namespace scripting {

ScriptManager::ScriptManager()
{
    base::glog << "Initializing Lua" << base::logmess;

    m_state = make_unique<sel::State>(true);
}

ScriptManager::~ScriptManager()
{

}

bool ScriptManager::doFile(const char *fileName)
{
    auto fullp = g_fileSystem->fullPath(fileName);

    base::glog << "Executing" << fullp << base::logmess;

    if (!m_state->Load(fullp))
    {
        base::glog << "Lua script execution failed." << base::logwarn;
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

sel::State &ScriptManager::getState()
{
    return *m_state;
}

} }
