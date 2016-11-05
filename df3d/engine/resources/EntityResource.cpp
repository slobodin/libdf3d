#include "EntityResource.h"

#include <df3d/lib/JsonUtils.h>

namespace df3d {

static void PreloadEntityData(const Json::Value &root, std::vector<ResourceID> &outDeps)
{
    const auto &componentsJson = root["components"];
    for (const auto &compJson : componentsJson)
    {
        auto type = compJson["type"].asString();
        const auto &data = compJson["data"];
        if (type == "mesh")
            outDeps.push_back(data["path"].asString());
        else if (type == "vfx")
            outDeps.push_back(data["path"].asString());
    }

    const auto &childrenJson = root["children"];
    for (const auto &child : childrenJson)
        PreloadEntityData(child, outDeps);
}

void EntityHolder::listDependencies(ResourceDataSource &dataSource, std::vector<ResourceID> &outDeps)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return;

    PreloadEntityData(root, outDeps);
}

bool EntityHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    m_resource = MAKE_NEW(allocator, EntityResource)();
    m_resource->root = std::move(root);

    return true;
}

void EntityHolder::decodeCleanup(Allocator &allocator)
{

}

bool EntityHolder::createResource(Allocator &allocator)
{
    return true;
}

void EntityHolder::destroyResource(Allocator &allocator)
{
    MAKE_DELETE(allocator, m_resource);
    m_resource = nullptr;
}

}
