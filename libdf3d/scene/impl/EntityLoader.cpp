#include "df3d_pch.h"
#include "EntityLoader.h"

#include <scene/World.h>
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

    return res;
}

} }
