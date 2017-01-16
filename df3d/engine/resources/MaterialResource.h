#pragma once

#include <df3d/engine/render/Material.h>
#include "IResourceHolder.h"

namespace df3d {

class MaterialLibResource
{
    std::unordered_map<Id, Material> m_materials;

    void parse(const rapidjson::Value &root);

public:
    MaterialLibResource(const rapidjson::Value &root);

    const Material* getMaterial(Id name) const;
};

class MaterialLibHolder : public IResourceHolder
{
    MaterialLibResource *m_resource = nullptr;
    rapidjson::Document *m_root = nullptr;

public:
    void listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps);
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

}

