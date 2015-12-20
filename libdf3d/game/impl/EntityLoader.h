#pragma once

#include <game/Entity.h>

namespace df3d { 

class World;

namespace scene_impl {

class EntityLoader
{
public:
    static Entity createEntity(const std::string &resourceFile, World &w);
    static Entity createEntity(const Json::Value &root, World &w);
};

} }
