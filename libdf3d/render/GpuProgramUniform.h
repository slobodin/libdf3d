#pragma once

namespace df3d { namespace render {

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

    // FIXME:
    // Calculate fog as post fx.
    FOG_DENSITY_UNIFORM,
    FOG_COLOR_UNIFORM,

    PIXEL_SIZE_UNIFORM,

    ELAPSED_TIME_UNIFORM,

    MATERIAL_AMBIENT_UNIFORM,
    MATERIAL_DIFFUSE_UNIFORM,
    MATERIAL_SPECULAR_UNIFORM,
    MATERIAL_EMISSIVE_UNIFORM,
    MATERIAL_SHININESS_UNIFORM,

    SCENE_LIGHT_DIFFUSE_UNIFORM,
    SCENE_LIGHT_SPECULAR_UNIFORM,
    SCENE_LIGHT_POSITION_UNIFORM,
    SCENE_LIGHT_KC_UNIFORM,
    SCENE_LIGHT_KL_UNIFORM,
    SCENE_LIGHT_KQ_UNIFORM,

    COUNT
};

class GpuProgramUniform
{
    friend class GpuProgram;

    std::string m_name;
    int m_location = -1;
    unsigned m_glType = 0;

    SharedUniformType m_sharedId = SharedUniformType::COUNT;
    bool m_isSampler = false;

    GpuProgramUniform(const std::string &name);

public:
    ~GpuProgramUniform();

    void update(const void *data) const;

    SharedUniformType getSharedType() const { return m_sharedId; }
    bool isShared() const { return m_sharedId != SharedUniformType::COUNT; }
    bool isSampler() const { return m_isSampler; }
    const std::string &getName() const { return m_name; }
};

} }