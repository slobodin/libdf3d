#include "df3d_pch.h"
#include "ComponentSerializer.h"

#include "TransformComponentSerializer.h"
#include "MeshComponentSerializer.h"
#include "ParticleSystemComponentSerializer.h"
#include "AudioComponentSerializer.h"
#include "PhysicsComponentSerializer.h"
#include "LightComponentSerializer.h"
#include "DebugDrawComponentSerializer.h"
#include "Sprite2DComponentSerializer.h"

namespace df3d { namespace components { namespace serializers {

unique_ptr<ComponentSerializer> create(ComponentType type)
{
    switch (type)
    {
    case TRANSFORM:
        return make_unique<TransformComponentSerializer>();
    case MESH:
        return make_unique<MeshComponentSerializer>();
    case PARTICLE_EFFECT:
        return make_unique<ParticleSystemComponentSerializer>();
    case AUDIO:
        return make_unique<AudioComponentSerializer>();
    case PHYSICS:
        return make_unique<PhysicsComponentSerializer>();
    case LIGHT:
        return make_unique<LightComponentSerializer>();
    case DEBUG_DRAW:
        return make_unique<DebugDrawComponentSerializer>();
    case SPRITE_2D:
        return make_unique<Sprite2DComponentSerializer>();
    default:
        break;
    }

    return nullptr;
}

} } }
