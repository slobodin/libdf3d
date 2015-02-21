#include "df3d_pch.h"
#include "ScriptComponent.h"

#include <base/SystemsMacro.h>

namespace df3d { namespace components {

void ScriptComponent::onAttached()
{
    if (!g_scriptManager->doFile(m_scriptFilePath.c_str()))
        base::glog << "Script component attached with error" << base::logwarn;
}

ScriptComponent::ScriptComponent(const char *scriptFilePath)
    : NodeComponent(SCRIPT),
    m_scriptFilePath(scriptFilePath)
{

}

ScriptComponent::~ScriptComponent()
{

}

shared_ptr<NodeComponent> ScriptComponent::clone() const
{
    // TODO:
    assert(false);

    return nullptr;
}

} }
