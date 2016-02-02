#pragma once

#include <libdf3d/game/EntityComponentLoader.h>
#include <libdf3d/physics/PhysicsComponentProcessor.h>
#include <libdf3d/physics/PhysicsComponentCreationParams.h>

namespace df3d {

class PhysicsComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        auto params = PhysicsComponentCreationParams(root);

        // NOTE: assuming that mesh already added to this entity.
        w.physics().add(e, params, w.staticMesh().getMeshData(e));
    }
};

}
