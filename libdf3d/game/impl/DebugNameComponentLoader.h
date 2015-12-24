#pragma once

#include <game/EntityComponentLoader.h>
#include <game/DebugNameComponentProcessor.h>

namespace df3d {

class DebugNameComponentLoader : public EntityComponentLoader
{
public:
    void loadComponent(const Json::Value &root, Entity e, World &w) const override
    {
        std::string name;
        root["name"] >> name;

        w.debugName().add(e, name);
    }
};

}
