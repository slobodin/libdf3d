#include "EntityResource.h"

#include <df3d/lib/JsonUtils.h>

namespace df3d {

static void PreloadEntityData(const rapidjson::Value &root, std::vector<std::string> &outDeps)
{
    if (root.HasMember("components"))
    {
        const auto &componentsJson = root["components"];
        for (const auto &compJson : componentsJson.GetArray())
        {
            auto type = Id(compJson["type"].GetString());

            DF3D_ASSERT(compJson.HasMember("data"));
            const auto &data = compJson["data"];

            if (type == Id("mesh"))
                outDeps.push_back(data["path"].GetString());
            else if (type == Id("vfx"))
                outDeps.push_back(data["path"].GetString());
        }
    }

    if (root.HasMember("children"))
    {
        const auto &childrenJson = root["children"];
        for (const auto &child : childrenJson.GetArray())
            PreloadEntityData(child, outDeps);
    }
}

void EntityHolder::listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.IsNull())
        return;

    PreloadEntityData(root, outDeps);
}

bool EntityHolder::decodeStartup(ResourceDataSource &dataSource, Allocator &allocator)
{
    auto root = JsonUtils::fromFile(dataSource);
    if (root.IsNull())
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
