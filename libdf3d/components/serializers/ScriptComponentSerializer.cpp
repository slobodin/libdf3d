#include "df3d_pch.h"
#include "ScriptComponentSerializer.h"

#include <components/ScriptComponent.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace components { namespace serializers {

shared_ptr<NodeComponent> ScriptComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<ScriptComponent>(root["path"].asCString());

    return result;
}

Json::Value ScriptComponentSerializer::toJson(shared_ptr<const NodeComponent> component)
{
    Json::Value result;

    auto comp = static_cast<const ScriptComponent*>(component.get());
    result["path"] = comp->getScriptFilePath();

    return result;
}

} } }