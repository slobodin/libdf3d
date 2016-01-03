#pragma once

#include <game/EntityComponentLoader.h>
#include <3d/SceneGraphComponentProcessor.h>

namespace df3d {

class SceneGraphComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        glm::vec3 position, rotation;
        glm::vec3 scale(1.0f, 1.0f, 1.0f);
        
        root["position"] >> position;
        root["rotation"] >> rotation;
        root["scale"] >> scale;

        // NOTE: assuming it's already added.
        w.sceneGraph().setPosition(e, position);
        w.sceneGraph().setOrientation(e, rotation);
        w.sceneGraph().setScale(e, scale);
    }
};

}
