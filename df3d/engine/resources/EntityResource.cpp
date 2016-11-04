#include "EntityResource.h"

#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/lib/JsonUtils.h>

namespace df3d {

static void PreloadEntityData(const Json::Value &root)
{
    const auto &componentsJson = root["components"];
    for (const auto &compJson : componentsJson)
    {
        auto type = compJson["type"].asString();
        const auto &data = compJson["data"];
        if (type == "mesh")
            svc().resourceManager().loadResource(data["path"].asString());
        else if (type == "vfx")
            svc().resourceManager().loadResource(data["path"].asString());
    }

    const auto &childrenJson = root["children"];
    for (const auto &child : childrenJson)
        PreloadEntityData(child);
}

bool EntityHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    Json::Value root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    m_resource = MAKE_NEW(allocator, EntityResource)();
    m_resource->root = std::move(root);

    PreloadEntityData(m_resource->root);

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
