#include "NodeComponent.h"

#include <scene/Node.h>

namespace df3d { namespace components {

static const std::map<std::string, ComponentType> NameType =
{
    { "transform", TRANSFORM },
    { "mesh", MESH },
    { "vfx", PARTICLE_EFFECT },
    { "audio", AUDIO },
    { "physics", PHYSICS },
    { "light", LIGHT },
    { "debug_draw", DEBUG_DRAW },
    { "sprite_2d", SPRITE_2D }
};

static const std::map<ComponentType, std::string> TypeName =
{
    { TRANSFORM, "transform"},
    { MESH, "mesh" },
    { PARTICLE_EFFECT, "vfx" },
    { AUDIO, "audio" },
    { PHYSICS, "physics" },
    { LIGHT, "light" },
    { DEBUG_DRAW, "debug_draw" },
    { SPRITE_2D, "sprite_2d" }
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

std::string NodeComponent::typeToString(ComponentType type)
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

} }
