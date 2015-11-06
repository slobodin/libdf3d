#pragma once

#include "ComponentSerializer.h"

namespace df3d { namespace component_serializers {

class DebugDrawComponentSerializer : public ComponentSerializer
{
public:
	shared_ptr<NodeComponent> fromJson(const Json::Value &root);
    Json::Value toJson(shared_ptr<NodeComponent> component);
};

} }
