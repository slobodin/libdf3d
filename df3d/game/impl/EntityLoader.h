#pragma once

#include <df3d/game/Entity.h>

namespace df3d { 

class World;
class EntityComponentLoader;

namespace game_impl {

class EntityLoader
{
    std::map<std::string, unique_ptr<EntityComponentLoader>> m_loaders;

public:
    EntityLoader();
    ~EntityLoader();

    Entity createEntity(const char *resourceFile, World &w);
    Entity createEntity(const Json::Value &root, World &w);

    void registerEntityComponentLoader(const std::string &name, unique_ptr<EntityComponentLoader> loader);
};

} }
