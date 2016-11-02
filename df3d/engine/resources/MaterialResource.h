#pragma once

#include <df3d/engine/render/Material.h>
#include "IResourceHolder.h"

namespace df3d {

class MaterialLibResource
{
    std::unordered_map<std::string, Material> m_materials;
    Json::Value m_root;
    // FIXME: resources are still being loaded while constructing material lib.
    bool m_initialized = false;

    void parse();

public:
    MaterialLibResource(const Json::Value &root);

    const Material* getMaterial(const std::string &name) const;
};

class MaterialLibHolder : public IResourceHolder
{
    MaterialLibResource *m_resource = nullptr;
    unique_ptr<Json::Value> m_root;

public:
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

}

