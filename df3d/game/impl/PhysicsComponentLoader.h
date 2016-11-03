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
        auto params = PhysicsComponentCreationParams(root);

        DF3D_ASSERT(!root["mesh"].isNull());
        w.physics().add(e, params, root["mesh"].asString());
    }
};

}
