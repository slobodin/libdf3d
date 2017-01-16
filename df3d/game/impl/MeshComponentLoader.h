#pragma once

#include <df3d/game/EntityComponentLoader.h>
#include <df3d/engine/3d/StaticMeshComponentProcessor.h>

namespace df3d {

class MeshComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const rapidjson::Value &root, Entity e, World &w) const override
    {
        if (!root.HasMember("path"))
        {
            DFLOG_WARN("Invalid mesh component description. Empty 'path' field");
            return;
        }

        Id resourceId(root["path"].GetString());
        w.staticMesh().add(e, resourceId);
    }
};

}
