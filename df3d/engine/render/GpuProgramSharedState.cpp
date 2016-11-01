#include "GpuProgramSharedState.h"

#include "IRenderBackend.h"
#include "RenderManager.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/TimeManager.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/resources/GpuProgramResource.h>
#include <df3d/game/World.h>

namespace df3d {

void GpuProgramSharedState::resetFlags()
{
    m_worldViewProjDirty = m_worldViewDirty = m_worldView3x3Dirty
        = m_normalDirty = m_viewInverseDirty = m_worldInverseDirty = true;
}

const glm::mat4& GpuProgramSharedState::getWorldMatrix()
{
    return m_worldMatrix;
}

const glm::mat4& GpuProgramSharedState::getViewMatrix()
{
    return m_viewMatrix;
}

const glm::mat4& GpuProgramSharedState::getProjectionMatrix()
{
    return m_projMatrix;
}

const glm::mat4& GpuProgramSharedState::getWorldViewProjectionMatrix()
{
    if (m_worldViewProjDirty)
    {
        m_worldViewProj = m_projMatrix * getWorldViewMatrix();
        m_worldViewProjDirty = false;
    }

    return m_worldViewProj;
}

const glm::mat4& GpuProgramSharedState::getWorldViewMatrix()
{
    if (m_worldViewDirty)
    {
        m_worldView = m_viewMatrix * m_worldMatrix;
        m_worldViewDirty = false;
    }

    return m_worldView;
}

const glm::mat3& GpuProgramSharedState::getWorldView3x3Matrix()
{
    if (m_worldView3x3Dirty)
    {
        m_worldView3x3 = glm::mat3(getWorldViewMatrix());
        m_worldView3x3Dirty = false;
    }

    return m_worldView3x3;
}

const glm::mat3& GpuProgramSharedState::getNormalMatrix()
{
    if (m_normalDirty)
    {
        m_normalMatrix = glm::inverseTranspose(getWorldView3x3Matrix());
        m_normalDirty = false;
    }

    return m_normalMatrix;
}

const glm::mat4& GpuProgramSharedState::getViewMatrixInverse()
{
    if (m_viewInverseDirty)
    {
        m_viewMatrixInverse = glm::inverse(m_viewMatrix);
        m_viewInverseDirty = false;
    }

    return m_viewMatrixInverse;
}

const glm::mat4& GpuProgramSharedState::getWorldMatrixInverse()
{
    if (m_worldInverseDirty)
    {
        m_worldMatrixInverse = glm::inverse(m_worldMatrix);
        m_worldInverseDirty = false;
    }

    return m_worldMatrixInverse;
}

void GpuProgramSharedState::setWorldMatrix(const glm::mat4 &worldm)
{
    m_worldMatrix = worldm;
    resetFlags();
}

void GpuProgramSharedState::setViewMatrix(const glm::mat4 &viewm)
{
    m_viewMatrix = viewm;
    resetFlags();
}

void GpuProgramSharedState::setProjectionMatrix(const glm::mat4 &projm)
{
    m_projMatrix = projm;
    // FIXME:
    // Not all flags have to be set. Whatever.
    resetFlags();
}

void GpuProgramSharedState::setViewPort(const Viewport &viewport)
{
    m_pixelSize = glm::vec2(1.0f / (float)viewport.width(), 1.0f / (float)viewport.height());
}

void GpuProgramSharedState::setFog(float density, const glm::vec3 &color)
{
    m_fogDensity = density;
    m_fogColor = glm::vec4(color, 1.0f);
}

void GpuProgramSharedState::setAmbientColor(const glm::vec3 &color)
{
    m_globalAmbient = glm::vec4(color, 1.0f);
}

void GpuProgramSharedState::setLight(const Light &light)
{
    // Update light params.
    m_currentLight.color = glm::vec4(light.getColor(), 1.0f) * light.getIntensity();

    // Since we calculate lighting in the view space we should translate position and direction.
    if (light.getType() == Light::Type::DIRECTIONAL)
    {
        auto dir = light.getDirection();
        m_currentLight.positionParam = getViewMatrix() * glm::vec4(dir, 0.0f);
    }
    else
    {
        DF3D_ASSERT_MESS(false, "unsupported"); // TODO:
    }
}

void GpuProgramSharedState::clear()
{
    resetFlags();

    m_worldMatrix = glm::mat4(1.0f);
    m_viewMatrix = glm::mat4(1.0f);
    m_projMatrix = glm::mat4(1.0f);

    m_cameraPosition = svc().defaultWorld().getCamera()->getPosition();

    m_engineElapsedTime = svc().timer().getElapsedTime();
}

void GpuProgramSharedState::updateSharedUniforms(const GpuProgramResource &program)
{
    for (const auto &sharedUni : program.sharedUniforms)
    {
        const void *data = nullptr;

        switch (sharedUni.type)
        {
        case SharedUniformType::WORLD_VIEW_PROJECTION_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldViewProjectionMatrix());
            break;
        case SharedUniformType::WORLD_VIEW_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldViewMatrix());
            break;
        case SharedUniformType::WORLD_VIEW_3X3_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldView3x3Matrix());
            break;
        case SharedUniformType::VIEW_INVERSE_MATRIX_UNIFORM:
            data = glm::value_ptr(getViewMatrixInverse());
            break;
        case SharedUniformType::VIEW_MATRIX_UNIFORM:
            data = glm::value_ptr(getViewMatrix());
            break;
        case SharedUniformType::WORLD_INVERSE_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldMatrixInverse());
            break;
        case SharedUniformType::WORLD_MATRIX_UNIFORM:
            data = glm::value_ptr(getWorldMatrix());
            break;
        case SharedUniformType::PROJECTION_MATRIX_UNIFORM:
            data = glm::value_ptr(getProjectionMatrix());
            break;
        case SharedUniformType::NORMAL_MATRIX_UNIFORM:
            data = glm::value_ptr(getNormalMatrix());
            break;
        case SharedUniformType::CAMERA_POSITION_UNIFORM:
            data = glm::value_ptr(m_cameraPosition);
            break;
        case SharedUniformType::GLOBAL_AMBIENT_UNIFORM:
            data = glm::value_ptr(m_globalAmbient);
            break;
        case SharedUniformType::FOG_DENSITY_UNIFORM:
            data = &m_fogDensity;
            break;
        case SharedUniformType::FOG_COLOR_UNIFORM:
            data = glm::value_ptr(m_fogColor);
            break;
        case SharedUniformType::PIXEL_SIZE_UNIFORM:
            data = glm::value_ptr(m_pixelSize);
            break;
        case SharedUniformType::ELAPSED_TIME_UNIFORM:
            data = &m_engineElapsedTime;
            break;
        case SharedUniformType::COUNT:
        default:
            break;
        }

        if (data)
            svc().renderManager().getBackend().setUniformValue(sharedUni.handle, data);
    }
}

void GpuProgramSharedState::updateSharedLightUniforms(const GpuProgramResource &program)
{
    for (const auto &sharedUni : program.sharedUniforms)
    {
        const void *data = nullptr;

        switch (sharedUni.type)
        {
        case SharedUniformType::SCENE_LIGHT_COLOR_UNIFORM:
            data = glm::value_ptr(m_currentLight.color);
            break;
        case SharedUniformType::SCENE_LIGHT_POSITION_UNIFORM:
            data = glm::value_ptr(m_currentLight.positionParam);
            break;
        default:
            break;
        }

        if (data)
            svc().renderManager().getBackend().setUniformValue(sharedUni.handle, data);
    }
}

}
