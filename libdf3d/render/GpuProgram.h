#pragma once

#include <resources/Resource.h>
#include "GpuProgramUniform.h"

namespace df3d { namespace render {

class Shader;

class GpuProgram : public resources::Resource
{
    friend class GpuProgramManualLoader;

    unsigned int m_programDescriptor = 0;

    std::vector<shared_ptr<Shader>> m_shaders;
    std::vector<GpuProgramUniform> m_sharedUniforms;
    std::vector<GpuProgramUniform> m_customUniforms;

    bool compileShaders();
    bool attachShaders();
    void requestUniforms();

    GpuProgram(const std::vector<shared_ptr<Shader>> &shaders);

public:
    ~GpuProgram();

    void bind();
    void unbind();

    const GpuProgramUniform &getSharedUniform(size_t idx) const { return m_sharedUniforms[idx]; }
    size_t getSharedUniformsCount() const { return m_sharedUniforms.size(); }

    GpuProgramUniform *getCustomUniform(const std::string &name);

    unsigned int descriptor() const { return m_programDescriptor; }
};

class GpuProgramManualLoader : public resources::ManualResourceLoader
{
    std::string m_resourceGuid;
    std::string m_vertexData;
    std::string m_fragmentData;

    std::string m_vertexShaderPath, m_fragmentShaderPath;

public:
    GpuProgramManualLoader(const std::string &guid, const std::string &vertexData, const std::string &fragmentData);
    GpuProgramManualLoader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    GpuProgram* load() override;
};

} }
