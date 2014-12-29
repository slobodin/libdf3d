#include "df3d_pch.h"
#include "ComponentSerializer.h"

#include "TransformComponentSerializer.h"
#include "MeshComponentSerializer.h"
#include "ParticleSystemComponentSerializer.h"
#include "AudioComponentSerializer.h"
#include "PhysicsComponentSerializer.h"
#include "LightComponentSerializer.h"
#include "ScriptComponentSerializer.h"

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
    case SCRIPT:
        return make_unique<ScriptComponentSerializer>();
    case DEBUG_DRAW:
        break;
    default:
        break;
    }

    return nullptr;
}

} } }