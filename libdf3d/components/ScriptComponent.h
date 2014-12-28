#pragma once

#include <components/NodeComponent.h>

namespace df3d { namespace components {

class DF3D_DLL ScriptComponent : public components::NodeComponent
{
    std::string m_scriptFilePath;

    void onAttached() override;

public:
    ScriptComponent(const char *scriptFilePath);
    ~ScriptComponent();

    const std::string &getScriptFilePath() const { return m_scriptFilePath; }

    shared_ptr<NodeComponent> clone() const override;
};

} }