#pragma once

#include "IResourceHolder.h"
#include <df3d/lib/JsonUtils.h>
#include <vector>
#include <string>

namespace df3d {

struct EntityResource
{
    Json::Value root;
};

class EntityHolder : public IResourceHolder
{
    EntityResource *m_resource = nullptr;

public:
    void listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps) override;
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

}
