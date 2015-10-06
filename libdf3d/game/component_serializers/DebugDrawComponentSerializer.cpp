#include "DebugDrawComponentSerializer.h"

#include <components/DebugDrawComponent.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

Component DebugDrawComponentSerializer::fromJson(const Json::Value &root)
{
    shared_ptr<components::DebugDrawComponent> result;
    auto typeStr = root["type"].asString();
    if (typeStr == "aabb")
        result = make_shared<components::DebugDrawComponent>(components::DebugDrawComponent::Type::AABB);
    else if (typeStr == "obb")
        result = make_shared<components::DebugDrawComponent>(components::DebugDrawComponent::Type::OBB);
    else
        base::glog << "Unknown debug draw component type" << typeStr << base::logwarn;

    return result;
}

Json::Value DebugDrawComponentSerializer::toJson(Component component)
{
    Json::Value result;

    // TODO:
    assert(false);

    return result;
}


} }
