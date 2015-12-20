#include "df3d_pch.h"
#include "EntityLoader.h"

#include <game/World.h>
#include <utils/JsonUtils.h>

namespace df3d { namespace scene_impl {

Entity EntityLoader::createEntity(const std::string &resourceFile, World &w)
{
    return createEntity(utils::json::fromFile(resourceFile), w);
}

Entity EntityLoader::createEntity(const Json::Value &root, World &w)
{
    auto res = w.spawn();

    if (root.empty())
    {
        glog << "Failed to init an entity from Json node" << logwarn;
        return res;
    }

    assert(false);

    //auto result = make_shared<ParticleSystemComponent>();

    //result->setWorldTransformed(utils::json::getOrDefault(root["worldTransformed"], true));
    //result->setSystemLifeTime(utils::json::getOrDefault(root["systemLifeTime"], -1.0f));

    //for (auto group : systemGroups)
    //    result->addSPKGroup(group);

    //result->initializeSPK();

    return res;
}

} }
