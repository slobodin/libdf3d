#pragma once

#include <components/NodeComponent.h>

namespace df3d { namespace component_serializers {

class ComponentSerializer
{
public:
    virtual ~ComponentSerializer() { }

    virtual shared_ptr<NodeComponent> fromJson(const Json::Value &root) = 0;
    virtual Json::Value toJson(shared_ptr<NodeComponent> component) = 0;
};

unique_ptr<ComponentSerializer> create(ComponentType type);

} }
