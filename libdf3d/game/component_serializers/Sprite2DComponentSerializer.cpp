#include "Sprite2DComponentSerializer.h"

#include <components/Sprite2DComponent.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace component_serializers {

shared_ptr<NodeComponent> Sprite2DComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<Sprite2DComponent>(root["path"].asString());

    return result;
}

Json::Value Sprite2DComponentSerializer::toJson(shared_ptr<NodeComponent> component)
{
    Json::Value result;

    // TODO:
    assert(false);

    return result;
}


} }
