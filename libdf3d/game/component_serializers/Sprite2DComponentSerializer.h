#pragma once

#include "ComponentSerializer.h"

namespace df3d { namespace component_serializers {

class Sprite2DComponentSerializer : public ComponentSerializer
{
public:
    Component fromJson(const Json::Value &root);
    Json::Value toJson(Component component);
};

} }
