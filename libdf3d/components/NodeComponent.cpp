#include "df3d_pch.h"
#include "NodeComponent.h"

#include <scene/Node.h>
#include <utils/JsonHelpers.h>
#include "serializers/ComponentSerializer.h"

namespace df3d { namespace components {

static const std::map<std::string, ComponentType> NameType = 
{
    { "transform", TRANSFORM },
    { "mesh", MESH },
    { "vfx", PARTICLE_EFFECT },
    { "audio", AUDIO },
    { "physics", PHYSICS },
    { "light", LIGHT },
    { "debug_draw", DEBUG_DRAW }
};

static const std::map<ComponentType, std::string> TypeName =
{
    { TRANSFORM, "transform"},
    { MESH, "mesh" },
    { PARTICLE_EFFECT, "vfx" },
    { AUDIO, "audio" },
    { PHYSICS, "physics" },
    { LIGHT, "light" },
    { DEBUG_DRAW, "debug_draw" }
};

NodeComponent::NodeComponent(ComponentType t)
    : type(t)
{

}

const char *NodeComponent::getName() const
{
    return TypeName.find(type)->second.c_str();
}

void NodeComponent::sendEvent(ComponentEvent ev)
{
    getHolder()->broadcastComponentEvent(this, ev);
}

shared_ptr<NodeComponent> NodeComponent::fromJson(const char *jsonFile)
{
    return fromJson(utils::jsonLoadFromFile(jsonFile));
}

shared_ptr<NodeComponent> NodeComponent::fromJson(const Json::Value &root)
{
    if (root.empty())
    {
        base::glog << "Failed to create a component. Json root is empty" << base::logwarn;
        return nullptr;
    }

    auto typeStr = root["type"].asCString();
    auto type = NameType.find(typeStr);
    if (type == NameType.end())
    {
        base::glog << "Failed to create a component of type" << typeStr << ". Unknown type" << base::logwarn;
        return nullptr;
    }

    const Json::Value &dataJson = root["data"];
    const Json::Value &externalDataJson = root["external_data"];
    if (dataJson.empty() && externalDataJson.empty())
    {
        base::glog << "Failed to init component" << typeStr << ". Empty \"data\" or \"external_data\" field" << base::logwarn;
        return nullptr;
    }

    auto serializer = serializers::create(type->second);
    if (!serializer)
    {
        base::glog << "Can not create component from json definition. Unsupported component type" << typeStr << base::logwarn;
        return nullptr;
    }

    if (dataJson.empty())
        return serializer->fromJson(utils::jsonLoadFromFile(externalDataJson.asCString()));
    else
        return serializer->fromJson(dataJson);
}

Json::Value NodeComponent::toJson(shared_ptr<const NodeComponent> component)
{
    Json::Value result;
    if (!component)
    {
        base::glog << "Failed to serialize a null component" << base::logwarn;
        return result;
    }

    result["type"] = component->getName();
    
    auto serializer = serializers::create(component->type);
    if (!serializer)
    {
        base::glog << "Failed to serialize a component: unsupported type" << base::logwarn;
        return nullptr;
    }

    result["data"] = serializer->toJson(component);

    return result;
}

} }