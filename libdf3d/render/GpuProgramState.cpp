#include "df3d_pch.h"
#include "GpuProgramState.h"

#include "GpuProgramUniform.h"
#include "RenderPass.h"
#include "OpenGLCommon.h"
#include <base/Controller.h>
#include <scene/SceneManager.h>
#include <scene/Camera.h>

namespace df3d { namespace render {

void GpuProgramState::resetFlags()
{
    m_worldViewProjDirty = m_worldViewDirty = m_worldView3x3Dirty
        = m_normalDirty = m_viewInverseDirty = m_worldInverseDirty = true;
}

GpuProgramState::GpuProgramState()
{
}

GpuProgramState::~GpuProgramState()
{
}

const glm::mat4 &GpuProgramState::getWorldMatrix()
{
    return m_worldMatrix;
}

const glm::mat4 &GpuProgramState::getViewMatrix()
{
    return m_viewMatrix;
}

const glm::mat4 &GpuProgramState::getProjectionMatrix()
{
    return m_projMatrix;
}

const glm::mat4 &GpuProgramState::getWorldViewProjectionMatrix()
{
    if (m_worldViewProjDirty)
    {
        m_worldViewProj = m_projMatrix * getWorldViewMatrix();
        m_worldViewProjDirty = false;
    }

    return m_worldViewProj;
}

const glm::mat4 &GpuProgramState::getWorldViewMatrix()
{
    if (m_worldViewDirty)
    {
        m_worldView = m_viewMatrix * m_worldMatrix;
        m_worldViewDirty = false;
    }

    return m_worldView;
}

const glm::mat3 &GpuProgramState::getWorldView3x3Matrix()
{
    if (m_worldView3x3Dirty)
    {
        m_worldView3x3 = glm::mat3(getWorldViewMatrix());
        m_worldView3x3Dirty = false;
    }

    return m_worldView3x3;
}

const glm::mat3 &GpuProgramState::getNormalMatrix()
{
    if (m_normalDirty)
    {
        m_normalMatrix = glm::inverseTranspose(getWorldView3x3Matrix());
        m_normalDirty = false;
    }

    return m_normalMatrix;
}

const glm::mat4 &GpuProgramState::getViewMatrixInverse()
{
    if (m_viewInverseDirty)
    {
        m_viewMatrixInverse = glm::inverse(m_viewMatrix);
        m_viewInverseDirty = false;
    }

    return m_viewMatrixInverse;
}

const glm::mat4 &GpuProgramState::getWorldMatrixInverse()
{
    if (m_worldInverseDirty)
    {
        m_worldMatrixInverse = glm::inverse(m_worldMatrix);
        m_worldInverseDirty = false;
    }

    return m_worldMatrixInverse;
}

void GpuProgramState::setWorldMatrix(const glm::mat4 &worldm)
{
    m_worldMatrix = worldm;
    resetFlags();
}

void GpuProgramState::setViewMatrix(const glm::mat4 &viewm)
{
    m_viewMatrix = viewm;
    resetFlags();
}

void GpuProgramState::setProjectionMatrix(const glm::mat4 &projm)
{
    m_projMatrix = projm;
    // FIXME:
    // Not all flags have to be set. Whatever.
    resetFlags();
}

void GpuProgramState::onFrameBegin()
{
    resetFlags();

    m_worldMatrix = glm::mat4(1.0f);
    m_viewMatrix = glm::mat4(1.0f);
    m_projMatrix = glm::mat4(1.0f);

    m_currentPass = nullptr;
    m_currentShader = nullptr;

    if (g_sceneManager->getCamera())
        m_cameraPosition = g_sceneManager->getCamera()->getPosition();
}

void GpuProgramState::onFrameEnd()
{
    glUseProgram(0);
}

void GpuProgramState::updateSharedUniform(const GpuProgramUniform &uniform)
{
    switch (uniform.getSharedType())
    {
    case WORLD_VIEW_PROJECTION_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldViewProjectionMatrix()));
        break;
    case WORLD_VIEW_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldViewMatrix()));
        break;
    case WORLD_VIEW_3X3_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldView3x3Matrix()));
        break;
    case VIEW_INVERSE_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getViewMatrixInverse()));
        break;
    case VIEW_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getViewMatrix()));
        break;
    case WORLD_INVERSE_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldMatrixInverse()));
        break;
    case WORLD_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getWorldMatrix()));
        break;
    case PROJECTION_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getProjectionMatrix()));
        break;
    case NORMAL_MATRIX_UNIFORM:
        uniform.update(glm::value_ptr(getNormalMatrix()));
        break;
    case CAMERA_POSITION_UNIFORM:
        uniform.update(glm::value_ptr(m_cameraPosition));
        break;
    case GLOBAL_AMBIENT_UNIFORM:
        uniform.update(glm::value_ptr(m_globalAmbient));
        break;
        break;
    case FOG_DENSITY_UNIFORM:
        uniform.update(&m_fogDensity);
        break;
    case FOG_COLOR_UNIFORM:
        uniform.update(glm::value_ptr(m_fogColor));
        break;
    case PIXEL_SIZE_UNIFORM:
        uniform.update(glm::value_ptr(m_pixelSize));
        break;
    case ELAPSED_TIME_UNIFORM:
    {
        float time = g_engineController->getElapsedTime();
        uniform.update(&time);
    }
        break;
    case MATERIAL_AMBIENT_UNIFORM:
        uniform.update(glm::value_ptr(m_currentPass->getAmbientColor()));
        break;
    case MATERIAL_DIFFUSE_UNIFORM:
        uniform.update(glm::value_ptr(m_currentPass->getDiffuseColor()));
        break;
    case MATERIAL_SPECULAR_UNIFORM:
        uniform.update(glm::value_ptr(m_currentPass->getSpecularColor()));
        break;
    case MATERIAL_EMISSIVE_UNIFORM:
        uniform.update(glm::value_ptr(m_currentPass->getEmissiveColor()));
        break;
    case MATERIAL_SHININESS_UNIFORM:
    {
        float sh = m_currentPass->getShininess();
        uniform.update(&sh);
    }
        break;
    case SCENE_LIGHT_DIFFUSE_UNIFORM:
        uniform.update(glm::value_ptr(m_currentLight.diffuseParam));
        break;
    case SCENE_LIGHT_SPECULAR_UNIFORM:
        uniform.update(glm::value_ptr(m_currentLight.specularParam));
        break;
    case SCENE_LIGHT_POSITION_UNIFORM:
        uniform.update(glm::value_ptr(m_currentLight.positionParam));
        break;
    case SCENE_LIGHT_KC_UNIFORM:
        uniform.update(&m_currentLight.k0Param);
        break;
    case SCENE_LIGHT_KL_UNIFORM:
        uniform.update(&m_currentLight.k1Param);
        break;
    case SCENE_LIGHT_KQ_UNIFORM:
        uniform.update(&m_currentLight.k2Param);
        break;
    case SHARED_UNIFORMS_COUNT:
    default:
        base::glog << "Can not set shared value to not shared uniform" << base::logwarn;
        break;
    }
}

} }
