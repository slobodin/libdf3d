#pragma once

#include <game/EntityComponentLoader.h>
#include <3d/TransformComponentProcessor.h>

namespace df3d {

class TransformComponentLoader : public EntityComponentLoader
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
        w.transform().setPosition(e, position);
        w.transform().setOrientation(e, rotation);
        w.transform().setScale(e, scale);
    }
};

}
