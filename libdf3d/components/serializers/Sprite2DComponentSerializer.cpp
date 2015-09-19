#include "Sprite2DComponentSerializer.h"

#include <components/Sprite2DComponent.h>
#include <utils/JsonHelpers.h>

namespace df3d { namespace components { namespace serializers {

shared_ptr<NodeComponent> Sprite2DComponentSerializer::fromJson(const Json::Value &root)
{
    auto result = make_shared<Sprite2DComponent>(root["path"].asCString());

    return result;
}

Json::Value Sprite2DComponentSerializer::toJson(shared_ptr<const NodeComponent> component)
{
    Json::Value result;

    // TODO:
    assert(false);

    return result;
}


} } }
