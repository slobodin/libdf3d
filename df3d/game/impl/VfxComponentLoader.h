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
        if (!root.isMember("path"))
        {
            DFLOG_WARN("Invalid vfx component description. Empty 'path' field");
            return;
        }

        Id resourceId(root["path"].asCString());
        w.vfx().addWithResource(e, resourceId);
    }
};

}
