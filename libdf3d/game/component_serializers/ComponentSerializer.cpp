#include "ComponentSerializer.h"

#include "TransformComponentSerializer.h"
#include "MeshComponentSerializer.h"
#include "ParticleSystemComponentSerializer.h"
#include "AudioComponentSerializer.h"
#include "PhysicsComponentSerializer.h"
#include "LightComponentSerializer.h"
#include "DebugDrawComponentSerializer.h"
#include "Sprite2DComponentSerializer.h"

namespace df3d { namespace component_serializers {

unique_ptr<ComponentSerializer> create(components::ComponentType type)
{
    switch (type)
    {
    case components::TRANSFORM:
        return make_unique<TransformComponentSerializer>();
    case components::MESH:
        return make_unique<MeshComponentSerializer>();
    case components::PARTICLE_EFFECT:
        return make_unique<ParticleSystemComponentSerializer>();
    case components::AUDIO:
        return make_unique<AudioComponentSerializer>();
    case components::PHYSICS:
        return make_unique<PhysicsComponentSerializer>();
    case components::LIGHT:
        return make_unique<LightComponentSerializer>();
    case components::DEBUG_DRAW:
        return make_unique<DebugDrawComponentSerializer>();
    case components::SPRITE_2D:
        return make_unique<Sprite2DComponentSerializer>();
    default:
        break;
    }

    return nullptr;
}

} }
