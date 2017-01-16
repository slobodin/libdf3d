#pragma once

#include <df3d/game/EntityComponentLoader.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>

namespace df3d {

class SceneGraphComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const rapidjson::Value &root, Entity e, World &w) const override
    {
        auto position = JsonUtils::get(root, "position", glm::vec3());
        auto rotation = JsonUtils::get(root, "rotation", glm::vec3());
        auto scale = JsonUtils::get(root, "scale", glm::vec3(1.0f, 1.0f, 1.0f));

        Id name;
        if (root.HasMember("name"))
            name = Id(root["name"].GetString());

        // NOTE: assuming it's already added.
        w.sceneGraph().setPosition(e, position);
        w.sceneGraph().setOrientation(e, rotation);
        w.sceneGraph().setScale(e, scale);
        w.sceneGraph().setName(e, name);
    }
};

}
