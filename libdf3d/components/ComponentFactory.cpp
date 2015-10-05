#include "ComponentFactory.h"

#include "serializers/ComponentSerializer.h"
#include <utils/JsonUtils.h>

namespace df3d { namespace components {

shared_ptr<NodeComponent> create(ComponentType type, const std::string &jsonFile)
{
    return create(type, utils::jsonLoadFromFile(jsonFile));
}

shared_ptr<NodeComponent> create(ComponentType type, const Json::Value &root)
{
    auto serializer = serializers::create(type);
    if (!serializer)
    {
        base::glog << "Can not create component from json definition. Unsupported component type" << base::logwarn;
        return nullptr;
    }

    return serializer->fromJson(root);
}

Json::Value toJson(shared_ptr<const NodeComponent> component)
{
    Json::Value result;
    if (!component)
    {
        base::glog << "Failed to serialize a null component" << base::logwarn;
        return result;
    }

    auto serializer = serializers::create(component->type);
    if (!serializer)
    {
        base::glog << "Failed to serialize a component: unsupported type" << base::logwarn;
        return nullptr;
    }

    return serializer->toJson(component);
}

} }
