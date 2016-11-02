#pragma once

#include <df3d/game/EntityComponentLoader.h>
#include <df3d/engine/particlesys/ParticleSystemComponentProcessor.h>
#include <df3d/engine/particlesys/ParticleSystemUtils.h>

namespace df3d {

class VfxComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        w.vfx().add(e, root["path"].asString());
    }
};

}
