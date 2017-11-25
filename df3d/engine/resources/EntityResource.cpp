#include "EntityResource.h"

#include <df3d/lib/JsonUtils.h>

namespace df3d {

static void PreloadEntityData(const Json::Value &root, std::vector<std::string> &outDeps)
{
    if (root.isMember("components"))
    {
        for (const auto &compJson : root["components"])
        {
            auto type = Id(compJson["type"].asCString());

            DF3D_ASSERT(compJson.isMember("data"));
            const auto &data = compJson["data"];

            if (type == Id("mesh"))
                outDeps.push_back(data["path"].asString());
            else if (type == Id("vfx"))
                outDeps.push_back(data["path"].asString());
        }
    }

    if (root.isMember("children"))
    {
        for (const auto &child : root["children"])
            PreloadEntityData(child, outDeps);
    }
}

void EntityHolder::listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return;

    PreloadEntityData(root, outDeps);
}

bool EntityHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.isNull())
        return false;

    m_resource = MAKE_NEW(allocator, EntityResource)();
    m_resource->root = std::move(root);
    m_resource->isWorld = m_isWorldResource;

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
