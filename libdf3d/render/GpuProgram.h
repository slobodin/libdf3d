#pragma once

#include <resources/Resource.h>
#include "GpuProgramUniform.h"

namespace df3d { namespace render {

class Shader;

const char * const SIMPLE_LIGHTING_PROGRAM_EMBED_PATH = "__embed_simple_lighting_program";
const char * const FFP2D_PROGRAM_EMBED_PATH = "__embed_ffp2d_program";
const char * const RTT_QUAD_PROGRAM_EMBED_PATH = "__embed_quad_render_program";
const char * const COLORED_PROGRAM_EMBED_PATH = "__embed_colored_program";
const char * const AMBIENT_PASS_PROGRAM_EMBED_PATH = "__embed_ambient_pass_program";

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

    bool init();
    
    const GpuProgramUniform &getSharedUniform(size_t idx) const { return m_sharedUniforms[idx]; }
    size_t getSharedUniformsCount() const { return m_sharedUniforms.size(); }

    GpuProgramUniform *getCustomUniform(const std::string &name);

    unsigned int descriptor() const { return m_programDescriptor; }

    static shared_ptr<GpuProgram> create(const char *vshader, const char *fshader);
    static shared_ptr<GpuProgram> createSimpleLightingGpuProgram();
    static shared_ptr<GpuProgram> createFFP2DGpuProgram();
};

} }