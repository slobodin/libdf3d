#pragma once

#include <SPARK.h>
#include "IResourceHolder.h"
#include <json/json/json.h>

namespace df3d {

struct ParticleSystemResource
{
    SPK::Ref<SPK::System> spkSystem;
};

class ParticleSystemHolder : public IResourceHolder
{
    ParticleSystemResource *m_resource = nullptr;
    Json::Value *m_root = nullptr;

public:
    void listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps) override;
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

}
