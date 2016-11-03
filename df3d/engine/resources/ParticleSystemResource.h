#pragma once

#include <SPARK.h>
#include "IResourceHolder.h"

namespace df3d {

struct ParticleSystemResource
{
    SPK::Ref<SPK::System> spkSystem;
};

class ParticleSystemHolder : public IResourceHolder
{
    ParticleSystemResource *m_resource = nullptr;
    Json::Value m_root;

public:
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

}
