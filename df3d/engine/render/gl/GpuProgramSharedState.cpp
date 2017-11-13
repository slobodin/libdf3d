#include <df3d/engine/render/IGpuProgramSharedState.h>

#include <df3d/engine/EngineController.h>
#include <df3d/engine/TimeManager.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/engine/resources/GpuProgramResource.h>
#include <df3d/engine/render/IRenderBackend.h>
#include <df3d/game/World.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace df3d {

class GpuProgramSharedStateDefault : public IGPUProgramSharedState
{
    glm::mat4 m_worldMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projMatrix;

    glm::mat4 m_worldViewProj;
    glm::mat4 m_worldView;
    glm::mat3 m_worldView3x3;
    glm::mat3 m_normalMatrix;

    glm::mat4 m_viewMatrixInverse;
    glm::mat4 m_worldMatrixInverse;

    glm::vec3 m_cameraPosition;

    struct GLSLLight
    {
        // NOTE: this vector is translated to the View Space. See OpenGLRenderer::setLight.
        glm::vec4 positionParam;
        glm::vec4 color;
    };

    GLSLLight m_lights[LIGHTS_MAX];
    glm::vec4 m_globalAmbient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    float m_fogDensity = 0.0f;
    glm::vec4 m_fogColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

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

    IRenderBackend *m_backend = nullptr;

    const glm::mat4& getWorldMatrix()
    {
        return m_worldMatrix;
    }

    const glm::mat4& getViewMatrix()
    {
        return m_viewMatrix;
    }

    const glm::mat4& getProjectionMatrix()
    {
        return m_projMatrix;
    }

    const glm::mat4& getWorldViewProjectionMatrix()
    {
        if (m_worldViewProjDirty)
        {
            m_worldViewProj = m_projMatrix * getWorldViewMatrix();
            m_worldViewProjDirty = false;
        }

        return m_worldViewProj;
    }

    const glm::mat4& getWorldViewMatrix()
    {
        if (m_worldViewDirty)
        {
            m_worldView = m_viewMatrix * m_worldMatrix;
            m_worldViewDirty = false;
        }

        return m_worldView;
    }

    const glm::mat3& getWorldView3x3Matrix()
    {
        if (m_worldView3x3Dirty)
        {
            m_worldView3x3 = glm::mat3(getWorldViewMatrix());
            m_worldView3x3Dirty = false;
        }

        return m_worldView3x3;
    }

    const glm::mat3& getNormalMatrix()
    {
        if (m_normalDirty)
        {
            m_normalMatrix = glm::inverseTranspose(getWorldView3x3Matrix());
            m_normalDirty = false;
        }

        return m_normalMatrix;
    }

    const glm::mat4& getViewMatrixInverse()
    {
        if (m_viewInverseDirty)
        {
            m_viewMatrixInverse = glm::inverse(m_viewMatrix);
            m_viewInverseDirty = false;
        }

        return m_viewMatrixInverse;
    }

    const glm::mat4& getWorldMatrixInverse()
    {
        if (m_worldInverseDirty)
        {
            m_worldMatrixInverse = glm::inverse(m_worldMatrix);
            m_worldInverseDirty = false;
        }

        return m_worldMatrixInverse;
    }

public:
    void setWorldMatrix(const glm::mat4 &worldm) override
    {
        m_worldMatrix = worldm;
        resetFlags();
    }

    void setViewMatrix(const glm::mat4 &viewm) override
    {
        m_viewMatrix = viewm;
        resetFlags();
    }

    void setProjectionMatrix(const glm::mat4 &projm) override
    {
        m_projMatrix = projm;

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
        m_fogColor = glm::vec4(color, 1.0f);
    }

    void setAmbientColor(const glm::vec3 &color) override
    {
        m_globalAmbient = glm::vec4(color, 1.0f);
    }

    void setLight(const Light &light, size_t idx) override
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

    void initialize(IRenderBackend *backend) override
    {
        m_backend = backend;

        resetFlags();

        m_worldMatrix = glm::mat4(1.0f);
        m_viewMatrix = glm::mat4(1.0f);
        m_projMatrix = glm::mat4(1.0f);

        m_cameraPosition = svc().defaultWorld().getCamera()->getPosition();

        m_engineElapsedTime = svc().timer().getElapsedTime();
    }

    void updateSharedUniforms(const GpuProgramResource &program) override
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
                    DF3D_ASSERT(false);
                    //data = glm::value_ptr(m_pixelSize);
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
                m_backend->setUniformValue(sharedUni.handle, data);
        }
    }
};

unique_ptr<IGPUProgramSharedState> IGPUProgramSharedState::create(RenderBackendID backendID)
{
    return make_unique<GpuProgramSharedStateDefault>();
}

}
