#pragma once

#include <df3d/game/EntityComponentLoader.h>
#include <df3d/engine/3d/StaticMeshComponentProcessor.h>

namespace df3d {

class MeshComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        w.staticMesh().add(e, root["path"].asString());
    }
};

}
