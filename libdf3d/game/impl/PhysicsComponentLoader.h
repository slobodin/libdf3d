#pragma once

#include <game/EntityComponentLoader.h>

namespace df3d {

class PhysicsComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {

    }
};

}
