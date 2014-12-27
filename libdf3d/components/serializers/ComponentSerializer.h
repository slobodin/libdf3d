#pragma once

#include <components/NodeComponent.h>

namespace df3d { namespace components { namespace serializers {

class ComponentSerializer
{
public:
    virtual ~ComponentSerializer() { }

    virtual shared_ptr<NodeComponent> fromJson(const Json::Value &root) = 0;
    virtual Json::Value toJson(shared_ptr<const NodeComponent> component) = 0;
};

unique_ptr<ComponentSerializer> create(ComponentType type);

} } }