#include "EntityLoader.h"

#include <game/World.h>
#include <3d/TransformComponentProcessor.h>
#include <utils/JsonUtils.h>
#include "AudioComponentLoader.h"
#include "MeshComponentLoader.h"
#include "PhysicsComponentLoader.h"
#include "Sprite2DComponentLoader.h"
#include "TransformComponentLoader.h"
#include "VfxComponentLoader.h"

namespace df3d { namespace scene_impl {

EntityLoader::EntityLoader()
{
    registerEntityComponentLoader("audio", make_unique<AudioComponentLoader>());
    registerEntityComponentLoader("transform", make_unique<TransformComponentLoader>());
    registerEntityComponentLoader("mesh", make_unique<MeshComponentLoader>());
    registerEntityComponentLoader("vfx", make_unique<VfxComponentLoader>());
    registerEntityComponentLoader("physics", make_unique<PhysicsComponentLoader>());
    registerEntityComponentLoader("sprite_2d", make_unique<Sprite2DComponentLoader>());
}

EntityLoader::~EntityLoader()
{

}

Entity EntityLoader::createEntity(const std::string &resourceFile, World &w)
{
    return createEntity(utils::json::fromFile(resourceFile), w);
}

Entity EntityLoader::createEntity(const Json::Value &root, World &w)
{
    if (root.empty())
    {
        glog << "Failed to init an entity from Json node" << logwarn;
        return Entity();
    }

    auto objName = root["name"].asString();
    if (objName.size())
    {
        // TODO_ecs:
        DEBUG_BREAK();
        assert(false);
    }

    const auto &componentsJson = root["components"];

    Entity res = w.spawn();
    for (const auto &componentJson : componentsJson)
    {
        const auto &dataJson = componentJson["data"];
        if (dataJson.empty())
        {
            glog << "Failed to init a component. Empty \"data\" field" << logwarn;
            w.destroy(res);
            return Entity();
        }

        if (!componentJson["external_data"].empty())
        {
            // TODO_ecs:
            DEBUG_BREAK();
            assert(false);
        }

        auto componentType = componentJson["type"].asString();
        auto foundLoader = m_loaders.find(componentType);
        if (foundLoader == m_loaders.end())
            glog << "Failed to parse entity description, unknown component" << componentType << logwarn;
        else
            foundLoader->second->loadComponent(dataJson, res, w);
    }

    const auto &childrenJson = root["children"];
    for (Json::UInt objIdx = 0; objIdx < childrenJson.size(); ++objIdx)
    {
        const auto &childJson = childrenJson[objIdx];
        w.transform().addChild(res, createEntity(childJson, w));
    }

    return res;
}

void EntityLoader::registerEntityComponentLoader(const std::string &name, unique_ptr<EntityComponentLoader> loader)
{
    m_loaders.insert(std::make_pair(name, std::move(loader)));
}

} }
