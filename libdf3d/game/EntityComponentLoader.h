#pragma once

#include <libdf3d/game/Entity.h>

namespace df3d {

class World;

class DF3D_DLL EntityComponentLoader
{
public:
    virtual ~EntityComponentLoader() = default;

    virtual void loadComponent(const Json::Value &root, Entity e, World &w) const = 0;
};

}
