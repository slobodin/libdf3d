#pragma once

#include <resources/Resource.h>
#include "GpuProgramUniform.h"

namespace df3d { namespace render {

class Shader;

class GpuProgram : public resources::Resource
{
    unsigned int m_programDescriptor = 0;
    bool m_shadersAttached = false;
    bool m_shadersCompiled = false;

    std::vector<shared_ptr<Shader>> m_shaders;
    std::vector<GpuProgramUniform> m_sharedUniforms;
    std::vector<GpuProgramUniform> m_customUniforms;

    void compileShaders();
    void attachShaders();
    void requestUniforms();

public:
    GpuProgram();
    ~GpuProgram();

    void attachShader(shared_ptr<Shader> shader);
    void detachShader(shared_ptr<Shader> shader);

    void bind();
    void unbind();

    const GpuProgramUniform &getSharedUniform(size_t idx) const { return m_sharedUniforms[idx]; }
    size_t getSharedUniformsCount() const { return m_sharedUniforms.size(); }

    GpuProgramUniform *getCustomUniform(const std::string &name);

    unsigned int descriptor() const { return m_programDescriptor; }
};

} }
