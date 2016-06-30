#pragma once

namespace df3d {

class GpuProgram;
class Light;
class Viewport;

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
        glm::vec4 diffuseParam;
        glm::vec4 specularParam;
        // NOTE: this vector is translated to the View Space. See OpenGLRenderer::setLight.
        glm::vec4 positionParam;
        float k0Param = 1.0f;
        float k1Param = 1.0f;
        float k2Param = 1.0f;
    };

    GLSLLight m_currentLight;
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
    void setLight(const Light &light);

    void clear();

    void updateSharedUniforms(const GpuProgram &program);
    void updateSharedLightUniforms(const GpuProgram &program);  // Forward rendering
};

}
