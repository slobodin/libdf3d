#include <df3d_pch.h>

#include <simd/simd.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace df3d {

class GpuProgramSharedStateMetal : public IGPUProgramSharedState
{
    static simd::float3 bridge(const glm::vec3& v)
    {
        return simd::float3{v.x, v.y, v.z};
    }

    static simd::float4 bridge(const glm::vec4& v)
    {
        return simd::float4{v.x, v.y, v.z, v.w};
    }

    static simd::float3x3 bridge(const glm::mat3& m)
    {
        return simd::float3x3(bridge(m[0]), bridge(m[1]), bridge(m[2]));
    }

    static simd::float4x4 bridge(const glm::mat4& m)
    {
        return simd::float4x4(bridge(m[0]), bridge(m[1]), bridge(m[2]), bridge(m[3]));
    }

    simd::float4x4 m_worldMatrix;
    simd::float4x4 m_viewMatrix;
    simd::float4x4 m_projMatrix;

    simd::float4x4 m_worldViewProj;
    simd::float4x4 m_worldView;
    simd::float3x3 m_worldView3x3;
    simd::float3x3 m_normalMatrix;

    simd::float4x4 m_viewMatrixInverse;
    simd::float4x4 m_worldMatrixInverse;

    simd::float3 m_cameraPosition;

    struct MetalLight
    {
        // NOTE: this vector is translated to the View Space. See OpenGLRenderer::setLight.
        simd::float4 positionParam;
        simd::float4 color;
    };

    MetalLight m_lights[LIGHTS_MAX];
    simd::float4 m_globalAmbient{0.2f, 0.2f, 0.2f, 1.0f};

    float m_fogDensity = 0.0f;
    simd::float4 m_fogColor{0.0f, 0.0f, 0.0f, 1.0f};

    float m_engineElapsedTime = 0.0f;

    bool m_worldViewProjDirty = true;
    bool m_worldViewDirty = true;
    bool m_worldView3x3Dirty = true;
    bool m_normalDirty = true;
    bool m_viewInverseDirty = true;
    bool m_worldInverseDirty = true;

    void resetFlags()
    {
        m_worldViewProjDirty = m_worldViewDirty = m_worldView3x3Dirty = m_normalDirty = m_viewInverseDirty = m_worldInverseDirty = true;
    }

    RenderBackendMetal *m_backend = nullptr;

    const simd::float4x4& getWorldMatrix()
    {
        return m_worldMatrix;
    }

    const simd::float4x4& getViewMatrix()
    {
        return m_viewMatrix;
    }

    const simd::float4x4& getProjectionMatrix()
    {
        return m_projMatrix;
    }

    const simd::float4x4& getWorldViewProjectionMatrix()
    {
        if (m_worldViewProjDirty)
        {
            m_worldViewProj = m_projMatrix * getWorldViewMatrix();
            m_worldViewProjDirty = false;
        }

        return m_worldViewProj;
    }

    const simd::float4x4& getWorldViewMatrix()
    {
        if (m_worldViewDirty)
        {
            m_worldView = m_viewMatrix * m_worldMatrix;
            m_worldViewDirty = false;
        }

        return m_worldView;
    }

    const simd::float3x3& getWorldView3x3Matrix()
    {
        if (m_worldView3x3Dirty)
        {
            simd::float4x4 wMatrix = getWorldViewMatrix();
            simd::float3 c1 = {wMatrix.columns[0].x, wMatrix.columns[0].y, wMatrix.columns[0].z};
            simd::float3 c2 = {wMatrix.columns[1].x, wMatrix.columns[1].y, wMatrix.columns[1].z};
            simd::float3 c3 = {wMatrix.columns[2].x, wMatrix.columns[2].y, wMatrix.columns[2].z};
            m_worldView3x3 = {c1,c2,c3};
            m_worldView3x3Dirty = false;
        }

        return m_worldView3x3;
    }

    const simd::float3x3& getNormalMatrix()
    {
        if (m_normalDirty)
        {
            m_normalMatrix = simd::inverse(simd::transpose(getWorldView3x3Matrix()));
            m_normalDirty = false;
        }

        return m_normalMatrix;
    }

    const simd::float4x4& getViewMatrixInverse()
    {
        if (m_viewInverseDirty)
        {
            m_viewMatrixInverse = simd::inverse(m_viewMatrix);
            m_viewInverseDirty = false;
        }

        return m_viewMatrixInverse;
    }

    const simd::float4x4& getWorldMatrixInverse()
    {
        if (m_worldInverseDirty)
        {
            m_worldMatrixInverse = simd::inverse(m_worldMatrix);
            m_worldInverseDirty = false;
        }

        return m_worldMatrixInverse;
    }

public:
    GpuProgramSharedStateMetal()
    {

    }

    ~GpuProgramSharedStateMetal()
    {

    }

    void setWorldMatrix(const glm::mat4 &worldm) override
    {
        m_worldMatrix = bridge(worldm);
        resetFlags();
    }

    void setViewMatrix(const glm::mat4 &viewm) override
    {
        m_viewMatrix = bridge(viewm);
        resetFlags();
    }

    void setProjectionMatrix(const glm::mat4 &projm) override
    {
        auto tmp = simd::float4x4(simd::float4{1.0f, 0.0f, 0.0f, 0.0f},
                                  simd::float4{0.0f, 1.0f, 0.0f, 0.0f},
                                  simd::float4{0.0f, 0.0f, 0.5f, 0.0f},
                                  simd::float4{0.0f, 0.0f, 0.5f, 1.0f});
        m_projMatrix = tmp * bridge(projm);

        // FIXME:
        // Not all flags have to be set. Whatever.
        resetFlags();
    }

    void setViewPort(const Viewport &viewport) override
    {
        // m_pixelSize = glm::vec2(1.0f / (float)viewport.width(), 1.0f / (float)viewport.height());
    }

    void setFog(float density, const glm::vec3 &color) override
    {
        m_fogDensity = density;
        m_fogColor = bridge(glm::vec4(color, 1.0f));
    }

    void setAmbientColor(const glm::vec3 &color) override
    {
        m_globalAmbient = bridge(glm::vec4(color, 1.0f));
    }

    void setLight(const Light &light, size_t idx) override
    {
        DF3D_ASSERT(idx < LIGHTS_MAX);
        // Update light params.
        auto tmpColor = glm::vec4(light.getColor(), 1.0f) * light.getIntensity();
        m_lights[idx].color = bridge(tmpColor);

        // Since we calculate lighting in the view space we should translate position and direction.
        if (light.getType() == Light::Type::DIRECTIONAL)
        {
            auto dir = light.getDirection();
            m_lights[idx].positionParam = getViewMatrix() * bridge(glm::vec4(dir, 0.0f));
        }
        else
        {
            DF3D_ASSERT_MESS(false, "not implemented"); // TODO:
        }
    }

    void initialize(IRenderBackend *backend) override
    {
        m_backend = static_cast<RenderBackendMetal*>(backend);

        resetFlags();

        m_worldMatrix = simd::float4x4(1.0f);
        m_viewMatrix = simd::float4x4(1.0f);
        m_projMatrix = simd::float4x4(1.0f);

        m_cameraPosition = bridge(svc().defaultWorld().getCamera()->getPosition());

        m_engineElapsedTime = svc().timer().getElapsedTime();
    }

    void updateSharedUniforms(const GpuProgramResource &program) override
    {
        auto gUniforms = m_backend->getGlobalUniforms();

        for (const auto &sharedUni : program.sharedUniforms)
        {
            switch (sharedUni.type)
            {
                case SharedUniformType::WORLD_VIEW_PROJECTION_MATRIX_UNIFORM:
                    gUniforms->u_worldViewProjectionMatrix = getWorldViewProjectionMatrix();
                    break;
                case SharedUniformType::WORLD_VIEW_MATRIX_UNIFORM:
                    gUniforms->u_worldViewMatrix = getWorldViewMatrix();
                    break;
                case SharedUniformType::WORLD_VIEW_3X3_MATRIX_UNIFORM:
                    gUniforms->u_worldViewMatrix3x3 = getWorldView3x3Matrix();
                    break;
                case SharedUniformType::VIEW_INVERSE_MATRIX_UNIFORM:
                    gUniforms->u_viewMatrixInverse = getViewMatrixInverse();
                    break;
                case SharedUniformType::VIEW_MATRIX_UNIFORM:
                    gUniforms->u_viewMatrix = getViewMatrix();
                    break;
                case SharedUniformType::WORLD_INVERSE_MATRIX_UNIFORM:
                    gUniforms->u_worldMatrixInverse = getWorldMatrixInverse();
                    break;
                case SharedUniformType::WORLD_MATRIX_UNIFORM:
                    gUniforms->u_worldMatrix = getWorldMatrix();
                    break;
                case SharedUniformType::PROJECTION_MATRIX_UNIFORM:
                    gUniforms->u_projectionMatrix = getProjectionMatrix();
                    break;
                case SharedUniformType::NORMAL_MATRIX_UNIFORM:
                    gUniforms->u_normalMatrix = getNormalMatrix();
                    break;
                case SharedUniformType::CAMERA_POSITION_UNIFORM:
                    gUniforms->u_cameraPosition = m_cameraPosition;
                    break;
                case SharedUniformType::GLOBAL_AMBIENT_UNIFORM:
                    gUniforms->u_globalAmbient = m_globalAmbient;
                    break;
                case SharedUniformType::FOG_DENSITY_UNIFORM:
                    gUniforms->u_fogDensity = m_fogDensity;
                    break;
                case SharedUniformType::FOG_COLOR_UNIFORM:
                    gUniforms->u_fogColor = m_fogColor;
                    break;
                case SharedUniformType::ELAPSED_TIME_UNIFORM:
                    gUniforms->u_elapsedTime = m_engineElapsedTime;
                    break;
                case SharedUniformType::SCENE_LIGHT_0_COLOR_UNIFORM:
                    gUniforms->light0.position = m_lights[0].positionParam;
                    gUniforms->light0.color = m_lights[0].color;
                    break;
                case SharedUniformType::SCENE_LIGHT_1_COLOR_UNIFORM:
                    gUniforms->light1.position = m_lights[1].positionParam;
                    gUniforms->light1.color = m_lights[1].color;
                    break;

                case SharedUniformType::COUNT:
                default:
                    DF3D_ASSERT(false);
                    break;
            }
        }
    }
};

unique_ptr<IGPUProgramSharedState> IGPUProgramSharedState::create(RenderBackendID backendID)
{
    return make_unique<GpuProgramSharedStateMetal>();
}

}
