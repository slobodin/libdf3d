#include "df3d_pch.h"
#include "GpuProgramUniform.h"

#include "OpenGLCommon.h"

namespace df3d { namespace render {

SharedUniformType getSharedTypeForUniform(const std::string &name)
{
    if (name == "WorldViewProjectionMatrix")
        return WORLD_VIEW_PROJECTION_MATRIX_UNIFORM;
    else if (name == "WorldViewMatrix")
        return WORLD_VIEW_MATRIX_UNIFORM;
    else if (name == "WorldViewMatrix3x3")
        return WORLD_VIEW_3X3_MATRIX_UNIFORM;
    else if (name == "ViewMatrixInverse")
        return VIEW_INVERSE_MATRIX_UNIFORM;
    else if (name == "ViewMatrix")
        return VIEW_MATRIX_UNIFORM;
    else if (name == "ProjectionMatrix")
        return PROJECTION_MATRIX_UNIFORM;
    else if (name == "WorldMatrix")
        return WORLD_MATRIX_UNIFORM;
    else if (name == "WorldMatrixInverse")
        return WORLD_INVERSE_MATRIX_UNIFORM;
    else if (name == "NormalMatrix")
        return NORMAL_MATRIX_UNIFORM;
    else if (name == "globalAmbient")
        return GLOBAL_AMBIENT_UNIFORM;
    else if (name == "CameraPosition")
        return CAMERA_POSITION_UNIFORM;
    else if (name == "FogDensity")
        return FOG_DENSITY_UNIFORM;
    else if (name == "FogColor")
        return FOG_COLOR_UNIFORM;
    else if (name == "PixelSize")
        return PIXEL_SIZE_UNIFORM;
    else if (name == "ElapsedTime")
        return ELAPSED_TIME_UNIFORM;
    else if (name == "material.ambient")
        return MATERIAL_AMBIENT_UNIFORM;
    else if (name == "material.diffuse")
        return MATERIAL_DIFFUSE_UNIFORM;
    else if (name == "material.specular")
        return MATERIAL_SPECULAR_UNIFORM;
    else if (name == "material.emissive")
        return MATERIAL_EMISSIVE_UNIFORM;
    else if (name == "material.shininess")
        return MATERIAL_SHININESS_UNIFORM;
    else if (name == "current_light.diffuse")
        return SCENE_LIGHT_DIFFUSE_UNIFORM;
    else if (name == "current_light.specular")
        return SCENE_LIGHT_SPECULAR_UNIFORM;
    else if (name == "current_light.position")
        return SCENE_LIGHT_POSITION_UNIFORM;
    else if (name == "current_light.constantAttenuation")
        return SCENE_LIGHT_KC_UNIFORM;
    else if (name == "current_light.linearAttenuation")
        return SCENE_LIGHT_KL_UNIFORM;
    else if (name == "current_light.quadraticAttenuation")
        return SCENE_LIGHT_KQ_UNIFORM;

    return SHARED_UNIFORMS_COUNT;
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