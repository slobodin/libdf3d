#pragma once

#include <game/EntityComponentLoader.h>
#include <physics/PhysicsComponentProcessor.h>
#include <physics/PhysicsComponentCreationParams.h>

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
