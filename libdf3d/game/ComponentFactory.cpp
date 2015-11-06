#include "ComponentFactory.h"

#include "component_serializers/ComponentSerializer.h"
#include <utils/JsonUtils.h>

namespace df3d {

shared_ptr<NodeComponent> componentFromFile(ComponentType type, const std::string &jsonFile)
{
    return componentFromJson(type, utils::json::fromFile(jsonFile));
}

shared_ptr<NodeComponent> componentFromJson(ComponentType type, const Json::Value &root)
{
    auto serializer = component_serializers::create(type);
    if (!serializer)
    {
        glog << "Can not create component from json definition. Unsupported component type" << logwarn;
        return nullptr;
    }

    return serializer->fromJson(root);
}

Json::Value saveComponent(shared_ptr<NodeComponent> component)
{
    Json::Value result;
    if (!component)
    {
        glog << "Failed to serialize a null component" << logwarn;
        return result;
    }

    auto serializer = component_serializers::create(component->type);
    if (!serializer)
    {
        glog << "Failed to serialize a component: unsupported type" << logwarn;
        return nullptr;
    }

    return serializer->toJson(component);
}

}
