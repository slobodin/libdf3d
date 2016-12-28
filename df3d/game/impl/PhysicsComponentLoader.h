#pragma once

#include <df3d/game/EntityComponentLoader.h>
#include <df3d/engine/physics/PhysicsComponentProcessor.h>
#include <df3d/engine/physics/PhysicsComponentCreationParams.h>

namespace df3d {

class PhysicsComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        if (!root.isMember("mesh"))
        {
            DFLOG_WARN("Invalid physics component description. Empty 'mesh' field");
            return;
        }

        auto params = PhysicsComponentCreationParams(root);

        w.physics().add(e, params, df3d::Id(root["mesh"].asCString()));
    }
};

}
