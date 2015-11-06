#include "NodeComponent.h"

#include <scene/Node.h>

namespace df3d {

static const std::map<std::string, ComponentType> NameType =
{
    { "transform", ComponentType::TRANSFORM },
    { "mesh", ComponentType::MESH },
    { "vfx", ComponentType::PARTICLE_EFFECT },
    { "audio", ComponentType::AUDIO },
    { "physics", ComponentType::PHYSICS },
    { "light", ComponentType::LIGHT },
    { "debug_draw", ComponentType::DEBUG_DRAW },
    { "sprite_2d", ComponentType::SPRITE_2D }
};

static const std::map<ComponentType, std::string> TypeName =
{
    { ComponentType::TRANSFORM, "transform"},
    { ComponentType::MESH, "mesh" },
    { ComponentType::PARTICLE_EFFECT, "vfx" },
    { ComponentType::AUDIO, "audio" },
    { ComponentType::PHYSICS, "physics" },
    { ComponentType::LIGHT, "light" },
    { ComponentType::DEBUG_DRAW, "debug_draw" },
    { ComponentType::SPRITE_2D, "sprite_2d" }
};

void NodeComponent::sendEvent(ComponentEvent ev)
{
    getHolder()->broadcastComponentEvent(this, ev);
}

NodeComponent::NodeComponent(ComponentType t)
    : type(t)
{

}

const std::string &NodeComponent::getName() const
{
    return typeToString(type);
}

const std::string& NodeComponent::typeToString(ComponentType type)
{
    return TypeName.find(type)->second;
}

ComponentType NodeComponent::stringToType(const std::string &typeStr)
{
    auto found = NameType.find(typeStr);
    if (found == NameType.end())
        return ComponentType::COUNT;
    else
        return found->second;
}

}
