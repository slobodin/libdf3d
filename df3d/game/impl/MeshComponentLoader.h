#pragma once

#include <df3d/game/EntityComponentLoader.h>
#include <df3d/engine/3d/StaticMeshComponentProcessor.h>

namespace df3d {

class MeshComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        if (!root.isMember("path"))
        {
            DFLOG_WARN("Invalid mesh component description. Empty 'path' field");
            return;
        }

        Id resourceId(root["path"].asCString());
        w.staticMesh().add(e, resourceId);
    }
};

}
