#include "ComponentFactory.h"

#include "component_serializers/ComponentSerializer.h"
#include <utils/JsonUtils.h>

namespace df3d {

using namespace components;

Component componentFromFile(ComponentType type, const std::string &jsonFile)
{
    return componentFromJson(type, utils::json::fromFile(jsonFile));
}

Component componentFromJson(ComponentType type, const Json::Value &root)
{
    auto serializer = component_serializers::create(type);
    if (!serializer)
    {
        base::glog << "Can not create component from json definition. Unsupported component type" << base::logwarn;
        return nullptr;
    }

    return serializer->fromJson(root);
}

Json::Value saveComponent(Component component)
{
    Json::Value result;
    if (!component)
    {
        base::glog << "Failed to serialize a null component" << base::logwarn;
        return result;
    }

    auto serializer = component_serializers::create(component->type);
    if (!serializer)
    {
        base::glog << "Failed to serialize a component: unsupported type" << base::logwarn;
        return nullptr;
    }

    return serializer->toJson(component);
}

}
