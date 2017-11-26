#pragma once

#include <df3d/game/EntityComponentLoader.h>
#include <df3d/game/TagComponentProcessor.h>

namespace df3d {

class TagComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        if (!root.isMember("tags"))
        {
            DFLOG_WARN("Invalid tag component description. Empty 'tags' field");
            return;
        }

        for (const auto &tagJson : root["tags"])
        {
            auto tag = df3d::Id(tagJson.asCString());
            w.tags().add(e, tag);
        }
    }
};

}
