#pragma once

#include "ComponentSerializer.h"

namespace df3d { namespace components { namespace serializers {

class TransformComponentSerializer : public ComponentSerializer
{
public:
    shared_ptr<NodeComponent> fromJson(const Json::Value &root);
    Json::Value toJson(shared_ptr<const NodeComponent> component);
};

} } }