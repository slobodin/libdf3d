#include "EntityLoader.h"

#include <df3d/game/World.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/lib/JsonUtils.h>
#include "AudioComponentLoader.h"
#include "MeshComponentLoader.h"
#include "PhysicsComponentLoader.h"
#include "Sprite2DComponentLoader.h"
#include "SceneGraphComponentLoader.h"
#include "VfxComponentLoader.h"

namespace df3d { namespace game_impl {

// A workaround for ordering of components loading (e.g. physics should be loaded after mesh).
// FIXME:
static std::map<std::string, int> LoadingPriority = {
    { "scenegraph", 10 },
    { "mesh", 9 }
};

EntityLoader::EntityLoader()
{
    registerEntityComponentLoader("audio", make_unique<AudioComponentLoader>());
    registerEntityComponentLoader("scenegraph", make_unique<SceneGraphComponentLoader>());
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
    return createEntity(JsonUtils::fromFile(resourceFile), w);
}

Entity EntityLoader::createEntity(const Json::Value &root, World &w)
{
    if (root.empty())
    {
        DFLOG_WARN("Failed to init an entity from Json node");
        return Entity();
    }

    const auto &componentsJson = root["components"];

    Entity res = w.spawn();

    struct LoadHandler
    {
        std::string type;
        std::function<void()> handler;
        bool operator<(const LoadHandler &other) const
        {
            return LoadingPriority[type] < LoadingPriority[other.type];
        }
    };

    std::priority_queue<LoadHandler> loadHandlers;
    for (const auto &componentJson : componentsJson)
    {
        const auto &dataJson = componentJson["data"];
        if (dataJson.empty())
        {
            DFLOG_WARN("Failed to init a component. Empty \"data\" field");
            w.destroy(res);
            return Entity();
        }

        auto componentType = componentJson["type"].asString();
        auto foundLoader = m_loaders.find(componentType);
        if (foundLoader == m_loaders.end())
            DFLOG_WARN("Failed to parse entity description, unknown component %s", componentType.c_str());
        else
            loadHandlers.push({ componentType, [&dataJson, res, &w, foundLoader]() { foundLoader->second->loadComponent(dataJson, res, w); } });
    }

    if (loadHandlers.empty())
        DFLOG_WARN("An entity has no components");

    while (!loadHandlers.empty())
    {
        loadHandlers.top().handler();
        loadHandlers.pop();
    }

    const auto &childrenJson = root["children"];
    for (auto &childJson : childrenJson)
        w.sceneGraph().attachChild(res, createEntity(childJson, w));

    return res;
}

void EntityLoader::registerEntityComponentLoader(const std::string &name, unique_ptr<EntityComponentLoader> loader)
{
    m_loaders.insert(std::make_pair(name, std::move(loader)));
}

} }
