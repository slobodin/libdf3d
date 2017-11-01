#pragma once

#include <df3d_pch.h>
#include "RenderCommon.h"

namespace df3d {

class Light;
class Viewport;
struct GpuProgramResource;
class IRenderBackend;

class GpuProgramSharedState
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

    glm::vec2 m_pixelSize = glm::vec2(1.0f / (float)DEFAULT_WINDOW_WIDTH, 1.0f / (float)DEFAULT_WINDOW_HEIGHT);

    float m_engineElapsedTime = 0.0f;

    bool m_worldViewProjDirty = true;
    bool m_worldViewDirty = true;
    bool m_worldView3x3Dirty = true;
    bool m_normalDirty = true;
    bool m_viewInverseDirty = true;
    bool m_worldInverseDirty = true;

    void resetFlags();

    IRenderBackend *m_backend = nullptr;

public:
    GpuProgramSharedState() = default;
    ~GpuProgramSharedState() = default;

    const glm::mat4& getWorldMatrix();
    const glm::mat4& getViewMatrix();
    const glm::mat4& getProjectionMatrix();
    const glm::mat4& getWorldViewProjectionMatrix();
    const glm::mat4& getWorldViewMatrix();
    const glm::mat3& getWorldView3x3Matrix();
    const glm::mat3& getNormalMatrix();
    const glm::mat4& getViewMatrixInverse();
    const glm::mat4& getWorldMatrixInverse();

    void setWorldMatrix(const glm::mat4 &worldm);
    void setViewMatrix(const glm::mat4 &viewm);
    void setProjectionMatrix(const glm::mat4 &projm);
    void setViewPort(const Viewport &viewport);

    void setFog(float density, const glm::vec3 &color);
    void setAmbientColor(const glm::vec3 &color);
    void setLight(const Light &light, size_t idx);

    void initialize(IRenderBackend *backend);

    void updateSharedUniforms(const GpuProgramResource &program);
};

}
