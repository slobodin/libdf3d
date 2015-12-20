#include "df3d_pch.h"
#include "EntityLoader.h"

#include <game/World.h>
#include <3d/TransformComponentProcessor.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace scene_impl {

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

        auto componentType = componentsJson["type"].asString();

        //shared_ptr<NodeComponent> component;
        //if (!externalDataJson.empty())
        //    component = componentFromFile(componentType, externalDataJson.asString());
        //else
        //    component = componentFromJson(componentType, dataJson);

        //result->attachComponent(component);
    }

    const auto &childrenJson = root["children"];
    for (Json::UInt objIdx = 0; objIdx < childrenJson.size(); ++objIdx)
    {
        const auto &childJson = childrenJson[objIdx];
        w.transform().addChild(res, createEntity(childJson, w));
    }

    return res;

    //auto result = make_shared<ParticleSystemComponent>();

    //result->setWorldTransformed(utils::json::getOrDefault(root["worldTransformed"], true));
    //result->setSystemLifeTime(utils::json::getOrDefault(root["systemLifeTime"], -1.0f));

    //for (auto group : systemGroups)
    //    result->addSPKGroup(group);

    //result->initializeSPK();

    return res;
}

} }
