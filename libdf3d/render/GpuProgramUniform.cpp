#include "GpuProgramUniform.h"

#include "OpenGLCommon.h"

namespace df3d { namespace render {

SharedUniformType getSharedTypeForUniform(const std::string &name)
{
    if (name == "WorldViewProjectionMatrix")
        return SharedUniformType::WORLD_VIEW_PROJECTION_MATRIX_UNIFORM;
    else if (name == "WorldViewMatrix")
        return SharedUniformType::WORLD_VIEW_MATRIX_UNIFORM;
    else if (name == "WorldViewMatrix3x3")
        return SharedUniformType::WORLD_VIEW_3X3_MATRIX_UNIFORM;
    else if (name == "ViewMatrixInverse")
        return SharedUniformType::VIEW_INVERSE_MATRIX_UNIFORM;
    else if (name == "ViewMatrix")
        return SharedUniformType::VIEW_MATRIX_UNIFORM;
    else if (name == "ProjectionMatrix")
        return SharedUniformType::PROJECTION_MATRIX_UNIFORM;
    else if (name == "WorldMatrix")
        return SharedUniformType::WORLD_MATRIX_UNIFORM;
    else if (name == "WorldMatrixInverse")
        return SharedUniformType::WORLD_INVERSE_MATRIX_UNIFORM;
    else if (name == "NormalMatrix")
        return SharedUniformType::NORMAL_MATRIX_UNIFORM;
    else if (name == "globalAmbient")
        return SharedUniformType::GLOBAL_AMBIENT_UNIFORM;
    else if (name == "CameraPosition")
        return SharedUniformType::CAMERA_POSITION_UNIFORM;
    else if (name == "FogDensity")
        return SharedUniformType::FOG_DENSITY_UNIFORM;
    else if (name == "FogColor")
        return SharedUniformType::FOG_COLOR_UNIFORM;
    else if (name == "PixelSize")
        return SharedUniformType::PIXEL_SIZE_UNIFORM;
    else if (name == "ElapsedTime")
        return SharedUniformType::ELAPSED_TIME_UNIFORM;
    else if (name == "material.ambient")
        return SharedUniformType::MATERIAL_AMBIENT_UNIFORM;
    else if (name == "material.diffuse")
        return SharedUniformType::MATERIAL_DIFFUSE_UNIFORM;
    else if (name == "material.specular")
        return SharedUniformType::MATERIAL_SPECULAR_UNIFORM;
    else if (name == "material.emissive")
        return SharedUniformType::MATERIAL_EMISSIVE_UNIFORM;
    else if (name == "material.shininess")
        return SharedUniformType::MATERIAL_SHININESS_UNIFORM;
    else if (name == "current_light.diffuse")
        return SharedUniformType::SCENE_LIGHT_DIFFUSE_UNIFORM;
    else if (name == "current_light.specular")
        return SharedUniformType::SCENE_LIGHT_SPECULAR_UNIFORM;
    else if (name == "current_light.position")
        return SharedUniformType::SCENE_LIGHT_POSITION_UNIFORM;
    else if (name == "current_light.constantAttenuation")
        return SharedUniformType::SCENE_LIGHT_KC_UNIFORM;
    else if (name == "current_light.linearAttenuation")
        return SharedUniformType::SCENE_LIGHT_KL_UNIFORM;
    else if (name == "current_light.quadraticAttenuation")
        return SharedUniformType::SCENE_LIGHT_KQ_UNIFORM;

    return SharedUniformType::COUNT;
}

GpuProgramUniform::GpuProgramUniform(const std::string &name)
    : m_name(name)
{
    assert(!m_name.empty());

    // Try to set it shared.
    m_sharedId = getSharedTypeForUniform(m_name);
}

GpuProgramUniform::~GpuProgramUniform()
{

}

void GpuProgramUniform::update(const void *data) const
{
    switch (m_glType)
    {
    case GL_INT:
        glUniform1iv(m_location, 1, (GLint *)data);
        break;
    case GL_FLOAT:
        glUniform1fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC2:
        glUniform2fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC3:
        glUniform3fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC4:
        glUniform4fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_MAT3:
        glUniformMatrix3fv(m_location, 1, GL_FALSE, (GLfloat *)data);
        break;
    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(m_location, 1, GL_FALSE, (GLfloat *)data);
        break;
    default:
        break;
    }
}

} }