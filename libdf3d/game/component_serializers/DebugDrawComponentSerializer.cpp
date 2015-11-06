#include "DebugDrawComponentSerializer.h"

#include <components/DebugDrawComponent.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

shared_ptr<NodeComponent> DebugDrawComponentSerializer::fromJson(const Json::Value &root)
{
    shared_ptr<DebugDrawComponent> result;
    auto typeStr = root["type"].asString();
    if (typeStr == "aabb")
        result = make_shared<DebugDrawComponent>(DebugDrawComponent::Type::AABB);
    else if (typeStr == "obb")
        result = make_shared<DebugDrawComponent>(DebugDrawComponent::Type::OBB);
    else
        glog << "Unknown debug draw component type" << typeStr << logwarn;

    return result;
}

Json::Value DebugDrawComponentSerializer::toJson(shared_ptr<NodeComponent> component)
{
    Json::Value result;

    // TODO:
    assert(false);

    return result;
}


} }
