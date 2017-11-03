#include <df3d_pch.h>
#include <df3d/df3d.h>
#include "GpuProgramSharedState.h"

#include "IRenderBackend.h"
#include "RenderManager.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/TimeManager.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/resources/GpuProgramResource.h>
#include <df3d/game/World.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef DF3D_IOS

#include <df3d/engine/render/metal/RenderBackendMetal.h>
#include <simd/simd.h>

namespace
{
    simd::float3 bridge(const glm::vec3& v) {
        return simd::float3{v.x, v.y, v.z};
    }
    simd::float4 bridge(const glm::vec4& v) {
        return simd::float4{v.x, v.y, v.z, v.w};
    }
    simd::float3x3 bridge(const glm::mat3& m) {
        return simd::float3x3(bridge(m[0]), bridge(m[1]), bridge(m[2]));
    }
    simd::float4x4 bridge(const glm::mat4& m) {
        return simd::float4x4(bridge(m[0]), bridge(m[1]), bridge(m[2]), bridge(m[3]));
    }
}

#endif

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
    if (m_backend->getID() == RenderBackendID::METAL)
    {
        auto tmp = glm::mat4(1.0f);
        tmp[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        tmp[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        tmp[2] = glm::vec4(0.0f, 0.0f, 0.5f, 0.0f);
        tmp[3] = glm::vec4(0.0f, 0.0f, 0.5f, 1.0f);

        m_projMatrix = tmp * m_projMatrix;
    }
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

void GpuProgramSharedState::setLight(const Light &light, size_t idx)
{
    DF3D_ASSERT(idx < LIGHTS_MAX);
    // Update light params.
    m_lights[idx].color = glm::vec4(light.getColor(), 1.0f) * light.getIntensity();

    // Since we calculate lighting in the view space we should translate position and direction.
    if (light.getType() == Light::Type::DIRECTIONAL)
    {
        auto dir = light.getDirection();
        m_lights[idx].positionParam = getViewMatrix() * glm::vec4(dir, 0.0f);
    }
    else
    {
        DF3D_ASSERT_MESS(false, "not implemented"); // TODO:
    }
}

void GpuProgramSharedState::initialize(IRenderBackend *backend)
{
    m_backend = backend;

    resetFlags();

    m_worldMatrix = glm::mat4(1.0f);
    m_viewMatrix = glm::mat4(1.0f);
    m_projMatrix = glm::mat4(1.0f);

    m_cameraPosition = svc().defaultWorld().getCamera()->getPosition();

    m_engineElapsedTime = svc().timer().getElapsedTime();
}

void GpuProgramSharedState::updateSharedUniforms(const GpuProgramResource &program)
{
    if (m_backend->getID() == RenderBackendID::GL)
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
                case SharedUniformType::SCENE_LIGHT_0_COLOR_UNIFORM:
                    data = glm::value_ptr(m_lights[0].color);
                    break;
                case SharedUniformType::SCENE_LIGHT_0_POSITION_UNIFORM:
                    data = glm::value_ptr(m_lights[0].positionParam);
                    break;
                case SharedUniformType::SCENE_LIGHT_1_COLOR_UNIFORM:
                    data = glm::value_ptr(m_lights[1].color);
                    break;
                case SharedUniformType::SCENE_LIGHT_1_POSITION_UNIFORM:
                    data = glm::value_ptr(m_lights[1].positionParam);
                    break;
                case SharedUniformType::COUNT:
                default:
                    break;
            }

            if (data)
                m_backend->setUniformValue(program.handle, sharedUni.handle, data);
        }
    }
#ifdef DF3D_IOS
    else if (m_backend->getID() == RenderBackendID::METAL)
    {
        auto gUniforms = static_cast<RenderBackendMetal*>(m_backend)->getGlobalUniforms();

        for (const auto &sharedUni : program.sharedUniforms)
        {
            switch (sharedUni.type)
            {
                case SharedUniformType::WORLD_VIEW_PROJECTION_MATRIX_UNIFORM:
                    gUniforms->u_worldViewProjectionMatrix = bridge( getWorldViewProjectionMatrix() );
                    break;
                case SharedUniformType::WORLD_VIEW_MATRIX_UNIFORM:
                    gUniforms->u_worldViewMatrix = bridge( getWorldViewMatrix() );
                    break;
                case SharedUniformType::WORLD_VIEW_3X3_MATRIX_UNIFORM:
                    gUniforms->u_worldViewMatrix3x3 = bridge( getWorldView3x3Matrix() );
                    break;
                case SharedUniformType::VIEW_INVERSE_MATRIX_UNIFORM:
                    gUniforms->u_viewMatrixInverse = bridge( getViewMatrixInverse() );
                    break;
                case SharedUniformType::VIEW_MATRIX_UNIFORM:
                    gUniforms->u_viewMatrix = bridge( getViewMatrix() );
                    break;
                case SharedUniformType::WORLD_INVERSE_MATRIX_UNIFORM:
                    gUniforms->u_worldMatrixInverse = bridge( getWorldMatrixInverse() );
                    break;
                case SharedUniformType::WORLD_MATRIX_UNIFORM:
                    gUniforms->u_worldMatrix = bridge( getWorldMatrix() );
                    break;
                case SharedUniformType::PROJECTION_MATRIX_UNIFORM:
                    gUniforms->u_projectionMatrix = bridge( getProjectionMatrix() );
                    break;
                case SharedUniformType::NORMAL_MATRIX_UNIFORM:
                    gUniforms->u_normalMatrix = bridge( getNormalMatrix() );
                    break;
                case SharedUniformType::CAMERA_POSITION_UNIFORM:
                    gUniforms->u_cameraPosition = bridge( m_cameraPosition );
                    break;
                case SharedUniformType::GLOBAL_AMBIENT_UNIFORM:
                    gUniforms->u_globalAmbient = bridge( m_globalAmbient );
                    break;
                case SharedUniformType::FOG_DENSITY_UNIFORM:
                    gUniforms->u_fogDensity = m_fogDensity;
                    break;
                case SharedUniformType::FOG_COLOR_UNIFORM:
                    gUniforms->u_fogColor = bridge( m_fogColor );
                    break;
                case SharedUniformType::ELAPSED_TIME_UNIFORM:
                    gUniforms->u_elapsedTime = m_engineElapsedTime;
                    break;
                case SharedUniformType::SCENE_LIGHT_0_COLOR_UNIFORM:
                    gUniforms->light0.position = bridge( m_lights[0].positionParam );
                    gUniforms->light0.color = bridge( m_lights[0].color );
                    break;
                case SharedUniformType::SCENE_LIGHT_1_COLOR_UNIFORM:
                    gUniforms->light1.position = bridge( m_lights[1].positionParam );
                    gUniforms->light1.color = bridge( m_lights[1].color );
                    break;

                case SharedUniformType::COUNT:
                default:
                    DF3D_ASSERT(false);
                    break;
            }
        }
    }
#endif
    else
    {
        DF3D_ASSERT(false);
    }
}

}
