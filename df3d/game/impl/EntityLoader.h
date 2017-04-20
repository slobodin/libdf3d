#pragma once

#include <df3d/game/Entity.h>
#include <df3d/lib/Id.h>
#include <unordered_map>
#include <json/json/json-forwards.h>
#include <df3d/Common.h>

namespace df3d { 

class World;
class EntityComponentLoader;

namespace game_impl {

class EntityLoader
{
    std::unordered_map<Id, unique_ptr<EntityComponentLoader>> m_loaders;

public:
    EntityLoader();
    ~EntityLoader();

    Entity createEntityFromFile(const char *resourceFile, World &w);
    Entity createEntityFromJson(const Json::Value &root, World &w);

    void registerEntityComponentLoader(Id name, unique_ptr<EntityComponentLoader> loader);
};

} }
