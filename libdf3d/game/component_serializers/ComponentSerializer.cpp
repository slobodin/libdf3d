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

unique_ptr<ComponentSerializer> create(ComponentType type)
{
    switch (type)
    {
	case ComponentType::TRANSFORM:
        return make_unique<TransformComponentSerializer>();
    case ComponentType::MESH:
        return make_unique<MeshComponentSerializer>();
    case ComponentType::PARTICLE_EFFECT:
        return make_unique<ParticleSystemComponentSerializer>();
    case ComponentType::AUDIO:
        return make_unique<AudioComponentSerializer>();
    case ComponentType::PHYSICS:
        return make_unique<PhysicsComponentSerializer>();
    case ComponentType::LIGHT:
        return make_unique<LightComponentSerializer>();
    case ComponentType::DEBUG_DRAW:
        return make_unique<DebugDrawComponentSerializer>();
    case ComponentType::SPRITE_2D:
        return make_unique<Sprite2DComponentSerializer>();
    default:
        break;
    }

    return nullptr;
}

} }
