#pragma once

#include <libdf3d/resources/Resource.h>
#include "RenderCommon.h"

namespace df3d {

enum class SharedUniformType
{
    WORLD_VIEW_PROJECTION_MATRIX_UNIFORM,
    WORLD_VIEW_MATRIX_UNIFORM,
    WORLD_VIEW_3X3_MATRIX_UNIFORM,
    VIEW_INVERSE_MATRIX_UNIFORM,
    VIEW_MATRIX_UNIFORM,
    WORLD_INVERSE_MATRIX_UNIFORM,
    WORLD_MATRIX_UNIFORM,
    NORMAL_MATRIX_UNIFORM,
    PROJECTION_MATRIX_UNIFORM,

    CAMERA_POSITION_UNIFORM,

    GLOBAL_AMBIENT_UNIFORM,

    FOG_DENSITY_UNIFORM,
    FOG_COLOR_UNIFORM,

    PIXEL_SIZE_UNIFORM,

    ELAPSED_TIME_UNIFORM,

    SCENE_LIGHT_DIFFUSE_UNIFORM,
    SCENE_LIGHT_SPECULAR_UNIFORM,
    SCENE_LIGHT_POSITION_UNIFORM,
    SCENE_LIGHT_KC_UNIFORM,
    SCENE_LIGHT_KL_UNIFORM,
    SCENE_LIGHT_KQ_UNIFORM,

    COUNT
};

struct SharedUniform
{
    SharedUniformType type;
    UniformDescriptor descr;
};

class GpuProgram : public Resource
{
    friend class GpuProgramManualLoader;

    GpuProgramDescriptor m_descriptor;
    std::vector<SharedUniform> m_sharedUniforms;

    GpuProgram(GpuProgramDescriptor descr);

public:
    ~GpuProgram();

    const std::vector<SharedUniform>& getSharedUniforms() const { return m_sharedUniforms; }

    GpuProgramDescriptor getDescriptor() const { return m_descriptor; }
};

class GpuProgramManualLoader : public ManualResourceLoader
{
    std::string m_resourceGuid;
    std::string m_vertexData;
    std::string m_fragmentData;

    std::string m_vertexShaderPath, m_fragmentShaderPath;

    ShaderDescriptor createShaderFromFile(const std::string &path) const;
    ShaderDescriptor createShaderFromString(const std::string &data, ShaderType shaderType) const;

public:
    GpuProgramManualLoader(const std::string &guid, const std::string &vertexData, const std::string &fragmentData);
    GpuProgramManualLoader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    GpuProgram* load() override;
};

}
