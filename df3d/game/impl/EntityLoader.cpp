#include "EntityLoader.h"

#include <df3d/game/World.h>
#include <df3d/engine/3d/SceneGraphComponentProcessor.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/EntityResource.h>
#include <df3d/lib/JsonUtils.h>
#include <df3d/lib/Utils.h>
#include "MeshComponentLoader.h"
#include "PhysicsComponentLoader.h"
#include "Sprite2DComponentLoader.h"
#include "SceneGraphComponentLoader.h"
#include "VfxComponentLoader.h"

namespace df3d { namespace game_impl {

EntityLoader::EntityLoader()
{
    registerEntityComponentLoader(Id("scenegraph"), make_unique<SceneGraphComponentLoader>());
    registerEntityComponentLoader(Id("mesh"), make_unique<MeshComponentLoader>());
    registerEntityComponentLoader(Id("vfx"), make_unique<VfxComponentLoader>());
    registerEntityComponentLoader(Id("physics"), make_unique<PhysicsComponentLoader>());
    registerEntityComponentLoader(Id("sprite_2d"), make_unique<Sprite2DComponentLoader>());
}

EntityLoader::~EntityLoader()
{

}

Entity EntityLoader::createEntityFromFile(const char *resourceFile, World &w)
{
    auto resource = svc().resourceManager().getResource<EntityResource>(Id(resourceFile));
    if (!resource)
        return {};

    return createEntityFromJson(resource->root, w);
}

Entity EntityLoader::createEntityFromJson(const rapidjson::Value &root, World &w)
{
    if (root.IsNull())
    {
        DFLOG_WARN("Failed to init an entity from Json node");
        return {};
    }

    Entity res = w.spawn();

    if (root.HasMember("components"))
    {
        auto componentsJson = root["components"].GetArray();
        for (const auto &it : componentsJson)
        {
            const auto &dataJson = it["data"];
            if (dataJson.IsNull())
            {
                DFLOG_WARN("Failed to init a component. Empty \"data\" field");
                w.destroy(res);
                return{};
            }

            auto componentType = it["type"].GetString();
            auto foundLoader = m_loaders.find(Id(componentType));
            if (foundLoader != m_loaders.end())
                foundLoader->second->loadComponent(dataJson, res, w);
            else
                DFLOG_WARN("Failed to parse entity description, unknown component %s", componentType);
        }
    }

    if (root.HasMember("children"))
    {
        auto childrenJson = root["children"].GetArray();
        for (const auto &it : childrenJson)
            w.sceneGraph().attachChild(res, createEntityFromJson(it, w));
    }

    return res;
}

void EntityLoader::registerEntityComponentLoader(Id name, unique_ptr<EntityComponentLoader> loader)
{
    DF3D_ASSERT(!utils::contains_key(m_loaders, name));
    m_loaders.insert(std::make_pair(name, std::move(loader)));
}

} }
