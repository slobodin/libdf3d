#pragma once

#include <game/EntityComponentLoader.h>

namespace df3d {

class Sprite2DComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {

    }
};

}
