#pragma once

#include <df3d/engine/resources/Resource.h>
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

    SCENE_LIGHT_COLOR_UNIFORM,
    SCENE_LIGHT_POSITION_UNIFORM,

    COUNT
};

struct SharedUniform
{
    SharedUniformType type;
    UniformHandle handle;
};

class GpuProgram : public Resource
{
    friend class GpuProgramManualLoader;

    GpuProgramHandle m_handle;
    df3d::PodArray<SharedUniform> m_sharedUniforms;

    std::unordered_map<std::string, UniformHandle> m_customUniforms;

    GpuProgram(GpuProgramHandle handle);

public:
    ~GpuProgram();

    const df3d::PodArray<SharedUniform>& getSharedUniforms() const { return m_sharedUniforms; }

    // NOTE: this method is supposed to be used rarely
    UniformHandle getCustomUniform(const std::string &name);
    std::vector<std::string> getCustomUniformNames() const;

    GpuProgramHandle getHandle() const { return m_handle; }
};

class GpuProgramManualLoader : public ManualResourceLoader
{
    std::string m_resourceGuid;
    std::string m_vertexData;
    std::string m_fragmentData;

    std::string m_vertexShaderPath, m_fragmentShaderPath;

    ShaderHandle createShaderFromFile(const std::string &path) const;
    ShaderHandle createShaderFromString(const std::string &data, ShaderType shaderType) const;

public:
    GpuProgramManualLoader(const std::string &guid, const std::string &vertexData, const std::string &fragmentData);
    GpuProgramManualLoader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    GpuProgram* load() override;
};

}
