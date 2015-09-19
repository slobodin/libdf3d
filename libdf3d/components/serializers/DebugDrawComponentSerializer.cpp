#include "DebugDrawComponentSerializer.h"

#include <components/DebugDrawComponent.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace components { namespace serializers {

shared_ptr<NodeComponent> DebugDrawComponentSerializer::fromJson(const Json::Value &root)
{
    shared_ptr<DebugDrawComponent> result;
    auto typeStr = root["type"].asString();
    if (typeStr == "aabb")
        result = make_shared<DebugDrawComponent>(DebugDrawComponent::Type::AABB);
    else if (typeStr == "obb")
        result = make_shared<DebugDrawComponent>(DebugDrawComponent::Type::OBB);
    else
        base::glog << "Unknown debug draw component type" << typeStr << base::logwarn;

    return result;
}

Json::Value DebugDrawComponentSerializer::toJson(shared_ptr<const NodeComponent> component)
{
    Json::Value result;

    // TODO:
    assert(false);

    return result;
}


} } }
