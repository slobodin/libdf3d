#include "df3d_pch.h"
#include "NodeComponent.h"

#include <scene/Node.h>
#include "PhysicsComponent.h"
#include "AudioComponent.h"
#include "ParticleSystemComponent.h"
#include "MeshComponent.h"
#include "TransformComponent.h"
#include "LightComponent.h"

namespace df3d { namespace components {

static const std::map<std::string, ComponentType> TypeName = 
{
    { "transform", ComponentType::TRANSFORM },
    { "mesh", ComponentType::MESH },
    { "vfx", ComponentType::PARTICLE_EFFECT },
    { "audio", ComponentType::AUDIO },
    { "physics", ComponentType::PHYSICS },
    { "light", ComponentType::LIGHT },
    { "debug_draw", ComponentType::DEBUG_DRAW }
};

static const std::map<ComponentType, std::string> TypeNameInv = // dunno how to name it
{
    { ComponentType::TRANSFORM, "transform" },
    { ComponentType::MESH, "mesh" },
    { ComponentType::PARTICLE_EFFECT, "vfx" },
    { ComponentType::AUDIO, "audio" },
    { ComponentType::PHYSICS, "physics" },
    { ComponentType::LIGHT, "light" },
    { ComponentType::DEBUG_DRAW, "debug_draw" }
};

NodeComponent::NodeComponent(ComponentType t)
    : type(t)
{

}

const char *NodeComponent::getName() const
{
    return TypeNameInv.find(type)->second.c_str();
}

void NodeComponent::sendEvent(ComponentEvent ev)
{
    getHolder()->broadcastComponentEvent(this, ev);
}

shared_ptr<NodeComponent> NodeComponent::create(const Json::Value &root)
{
    if (root.empty())
    {
        base::glog << "Failed to create component. Json root is empty" << base::logwarn;
        return nullptr;
    }

    auto typeStr = root["type"].asCString();
    auto type = TypeName.find(typeStr);
    if (type == TypeName.end())
    {
        base::glog << "Failed to create component of a type" << typeStr << ". Unknown type" << base::logwarn;
        return nullptr;
    }

    if (root["data"].empty())
    {
        base::glog << "Failed to init component" << typeStr << ". Invalid data field" << base::logwarn;
        return nullptr;
    }

    switch (type->second)
    {
    case ComponentType::TRANSFORM:
        return make_shared<TransformComponent>(root["data"]);
    case ComponentType::MESH:
        return make_shared<MeshComponent>(root["data"]["path"].asCString());
    case ComponentType::PARTICLE_EFFECT:
        return make_shared<ParticleSystemComponent>(root["data"]["path"].asCString());
    case ComponentType::AUDIO:
        return make_shared<AudioComponent>(root["data"]);
    case ComponentType::PHYSICS:
        return make_shared<PhysicsComponent>(root["data"]["path"].asCString());
    case ComponentType::LIGHT:
        return make_shared<LightComponent>(root["data"]);
    case ComponentType::DEBUG_DRAW:
        break;
    default:
        break;
    }

    base::glog << "Unsupported component type" << typeStr << base::logwarn;
    return nullptr;
}

} }