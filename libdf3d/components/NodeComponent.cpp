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
    std::make_pair("transform", CT_TRANSFORM),
    std::make_pair("mesh", CT_MESH),
    std::make_pair("vfx", CT_PARTICLE_EFFECT),
    std::make_pair("audio", CT_AUDIO),
    std::make_pair("physics", CT_PHYSICS),
    std::make_pair("light", CT_LIGHT),
    std::make_pair("debug_draw", CT_DEBUG_DRAW)
};

static const std::map<ComponentType, std::string> TypeNameInv = // dunno how to name it
{
    std::make_pair(CT_TRANSFORM, "transform"),
    std::make_pair(CT_MESH, "mesh"),
    std::make_pair(CT_PARTICLE_EFFECT, "vfx"),
    std::make_pair(CT_AUDIO, "audio"),
    std::make_pair(CT_PHYSICS, "physics"),
    std::make_pair(CT_LIGHT, "light"),
    std::make_pair(CT_DEBUG_DRAW, "debug_draw")
};

NodeComponent::NodeComponent(ComponentType t)
    : type(t)
{

}

const char *NodeComponent::getName() const
{
    return TypeNameInv.find(type)->second.c_str();
}

void NodeComponent::sendEvent(Event ev)
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
    case CT_TRANSFORM:
        return make_shared<TransformComponent>(root["data"]);
    case CT_MESH:
        return make_shared<MeshComponent>(root["data"]["path"].asCString());
    case CT_PARTICLE_EFFECT:
        return make_shared<ParticleSystemComponent>(root["data"]["path"].asCString());
    case CT_AUDIO:
        return make_shared<AudioComponent>(root["data"]);
    case CT_PHYSICS:
        return make_shared<PhysicsComponent>(root["data"]["path"].asCString());
    case CT_LIGHT:
        return make_shared<LightComponent>(root["data"]);
    case CT_DEBUG_DRAW:
        break;
    default:
        break;
    }

    base::glog << "Unsupported component type" << typeStr << base::logwarn;
    return nullptr;
}

} }