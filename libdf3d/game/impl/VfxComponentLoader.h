#pragma once

#include <libdf3d/game/EntityComponentLoader.h>
#include <libdf3d/particlesys/ParticleSystemComponentProcessor.h>

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
