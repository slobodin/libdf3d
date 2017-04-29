#pragma once

#include <df3d/engine/render/RenderCommon.h>
#include "IResourceHolder.h"

namespace df3d {

struct SharedUniform
{
    SharedUniformType type;
    UniformHandle handle;
};

struct GpuProgramResource
{
    GpuProgramHandle handle;
    df3d::PodArray<SharedUniform> sharedUniforms;
    std::unordered_map<Id, UniformHandle> customUniforms;

    GpuProgramResource(Allocator &allocator) : sharedUniforms(allocator) { }
};

class GpuProgramHolder : public IResourceHolder
{
    GpuProgramResource *m_resource = nullptr;
    std::string m_vShaderPath, m_fShaderPath;

public:
    void listDependencies(ResourceDataSource &dataSource, std::vector<std::string> &outDeps) override { }
    bool decodeStartup(ResourceDataSource &dataSource, Allocator &allocator) override;
    void decodeCleanup(Allocator &allocator) override;
    bool createResource(Allocator &allocator) override;
    void destroyResource(Allocator &allocator) override;

    void* getResource() override { return m_resource; }
};

GpuProgramResource* GpuProgramFromData(const std::string &vShaderData, const std::string &fShaderData, Allocator &alloc);

}
